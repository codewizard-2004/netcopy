"""Ask endpoint for querying Gemini and OpenAI."""

import os
import asyncio
import re
from fastapi import APIRouter, Request
from fastapi.responses import PlainTextResponse, StreamingResponse
from google import genai
from openai import AsyncOpenAI

router = APIRouter()

GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
OPENAI_API_KEY = os.getenv("OPENAI_API_KEY")

gemini_client = genai.Client(api_key=GEMINI_API_KEY) if GEMINI_API_KEY else None
openai_client = AsyncOpenAI(api_key=OPENAI_API_KEY) if OPENAI_API_KEY else None

LANGUAGE_COMMENT_PREFIXES = {
    "python": "#",
    "py": "#",
    "javascript": "//",
    "js": "//",
    "typescript": "//",
    "ts": "//",
    "c": "//",
    "c++": "//",
    "cpp": "//",
    "java": "//",
    "go": "//",
    "rust": "//",
    "php": "//",
    "ruby": "#",
    "bash": "#",
    "sh": "#",
    "shell": "#",
    "sql": "--",
    "html": "<!--",
    "css": "/*",
    "json": "//",
    "yaml": "#",
    "yml": "#",
    "swift": "//",
    "kotlin": "//",
    "csharp": "//",
    "cs": "//",
    "plaintext": "#",
}


def detect_language(query: str) -> str:
    """Infer likely target language from the user request."""
    text = query.lower().strip()
    patterns = {
        "python": [r"\bpython\b", r"\bpy\b", r"\.py\b"],
        "javascript": [r"\bjavascript\b", r"\bjs\b", r"\.js\b"],
        "typescript": [r"\btypescript\b", r"\bts\b", r"\.ts\b"],
        "c": [r"\bc\b", r"\.c\b"],
        "c++": [r"\bc\+\+\b", r"\bcpp\b", r"\.cpp\b"],
        "java": [r"\bjava\b", r"\.java\b"],
        "go": [r"\bgo\b", r"\bgolang\b", r"\.go\b"],
        "rust": [r"\brust\b", r"\.rs\b"],
        "php": [r"\bphp\b", r"\.php\b"],
        "ruby": [r"\bruby\b", r"\.rb\b"],
        "bash": [r"\bbash\b", r"\bshell\b", r"\bsh\b", r"\.sh\b"],
        "sql": [r"\bsql\b", r"\.sql\b"],
        "html": [r"\bhtml\b", r"\.html\b"],
        "css": [r"\bcss\b", r"\.css\b"],
        "json": [r"\bjson\b", r"\.json\b"],
        "yaml": [r"\byaml\b", r"\byml\b", r"\.ya?ml\b"],
        "swift": [r"\bswift\b", r"\.swift\b"],
        "kotlin": [r"\bkotlin\b", r"\.kt\b"],
        "csharp": [r"\bcsharp\b", r"\bcs\b", r"\.cs\b"],
    }

    for language, regexes in patterns.items():
        if any(re.search(pattern, text) for pattern in regexes):
            return language

    return "python"


def build_code_prompt(query: str, language: str) -> str:
    """For /code, force code-only output and keep explanations as comments."""
    return (
        f"You are generating {language} code only. "
        f"Do not include markdown fences, headers, preamble, disclaimers, or any text outside the code. "
        f"If you need to explain anything, write it as comments in {language} syntax. "
        f"Return only valid, runnable code.\n\n"
        f"Request: {query}"
    )


def strip_markdown_code_block(text: str) -> str:
    """Remove markdown fences from generated output."""
    cleaned = text.strip()
    cleaned = re.sub(r"^```[a-zA-Z0-9_+-]*\s*", "", cleaned)
    cleaned = re.sub(r"\s*```\s*$", "", cleaned)
    return cleaned.strip()


def language_comment(text: str, language: str) -> str:
    """Wrap explanatory text as a comment using the requested language."""
    prefix = LANGUAGE_COMMENT_PREFIXES.get(language, "#")
    if prefix in ("/*", "<!--"):
        return f"{prefix}\n{text}\n*/" if prefix == "/*" else f"{prefix}\n{text}\n-->"
    return "\n".join(f"{prefix} {line}" for line in text.splitlines()) if text else f"{prefix} "


def sanitize_code_output(text: str, language: str) -> str:
    """Return only code and bury explanation as comments when needed."""
    cleaned = strip_markdown_code_block(text)
    if not cleaned:
        return language_comment("No code was generated for this request.", language)

    if re.search(r"\b(if|for|while|def|class|function|const|let|var|return|import|from|package|public|class|SELECT|INSERT|UPDATE|DELETE)\b", cleaned, flags=re.IGNORECASE):
        return cleaned

    return language_comment(cleaned, language)


def _gemini_sync(query: str) -> dict:
    """Synchronous Gemini call (offloaded to thread)."""
    try:
        if not gemini_client:
            return {"success": False, "error": "Gemini API key not configured", "service": "Gemini"}

        response = gemini_client.models.generate_content(
            model="gemini-2.5-flash",
            contents=query
        )
        return {"success": True, "text": response.text, "service": "Gemini"}
    except Exception as e:
        error_msg = f"{type(e).__name__}: {str(e)}"
        print(f"Gemini Error: {error_msg}")  # Debug logging
        return {"success": False, "error": error_msg, "service": "Gemini"}


async def query_gemini(query: str) -> dict:
    """Query Google Gemini API without blocking the event loop."""
    return await asyncio.to_thread(_gemini_sync, query)


async def query_openai(query: str) -> dict:
    """Query OpenAI API as fallback."""
    try:
        if not openai_client:
            return {"success": False, "error": "OpenAI API key not configured", "service": "OpenAI"}

        response = await openai_client.chat.completions.create(
            model="gpt-5.4-nano",
            messages=[{"role": "user", "content": query}],
            temperature=0.7
        )
        return {"success": True, "text": response.choices[0].message.content, "service": "OpenAI"}
    except Exception as e:
        error_msg = f"{type(e).__name__}: {str(e)}"
        print(f"OpenAI Error: {error_msg}")  # Debug logging
        return {"success": False, "error": error_msg, "service": "OpenAI"}


def format_response(result: dict, is_backup: bool = False) -> str:
    """Return only the model response without terminal-only metadata."""
    return (result["text"] or "").strip()


async def response_generator(query: str, *, code_mode: bool = False, language: str = "python"):
    if not GEMINI_API_KEY and not OPENAI_API_KEY:
        yield "Error: No API keys configured. Set GEMINI_API_KEY or OPENAI_API_KEY in .env"
        return

    if GEMINI_API_KEY:
        task = asyncio.create_task(query_gemini(query))
    else:
        task = asyncio.create_task(query_openai(query))

    result = await task

    if result["success"]:
        if code_mode:
            yield sanitize_code_output(result["text"], language)
            return
        yield format_response(result, is_backup=False)
        return

    if GEMINI_API_KEY and OPENAI_API_KEY:
        result = await query_openai(query)
        if result["success"]:
            if code_mode:
                yield sanitize_code_output(result["text"], language)
                return
            yield format_response(result, is_backup=True)
            return
        yield f"Error querying {result['service']}: {result.get('error', 'Unknown error')}"
        return

    yield f"Error querying {result['service']}: {result.get('error', 'Unknown error')}"


@router.post("/ask")
async def ask(request: Request):
    """Ask Gemini or OpenAI a general question."""
    body = await request.body()
    query = body.decode("utf-8").strip()

    if not query:
        return PlainTextResponse("No query provided. Use: curl -d 'your question' localhost:8000/ask")

    return StreamingResponse(response_generator(query), media_type="text/plain")


@router.post("/code")
async def code(request: Request):
    """Generate code-only output, with comments used for any explanatory text."""
    body = await request.body()
    query = body.decode("utf-8").strip()

    if not query:
        return PlainTextResponse("No code request provided. Use: curl -d 'write a python function for factorial' localhost:8000/code")

    language = detect_language(query)
    prompt = build_code_prompt(query, language)
    return StreamingResponse(response_generator(prompt, code_mode=True, language=language), media_type="text/plain")