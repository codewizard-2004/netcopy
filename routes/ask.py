"""Ask endpoint for querying Gemini and OpenAI."""

import os
import asyncio
from fastapi import APIRouter, Request
from fastapi.responses import PlainTextResponse, StreamingResponse
from google import genai
from openai import AsyncOpenAI

router = APIRouter()

GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
OPENAI_API_KEY = os.getenv("OPENAI_API_KEY")

gemini_client = genai.Client(api_key=GEMINI_API_KEY) if GEMINI_API_KEY else None
openai_client = AsyncOpenAI(api_key=OPENAI_API_KEY) if OPENAI_API_KEY else None


def parse_provider_override(query: str) -> tuple[str, str]:
    """Return the cleaned query and an optional model override flag."""
    text = query.strip()
    if not text:
        return "", "auto"

    parts = text.split(maxsplit=1)
    flag = parts[0].lower()
    if flag in {"-oai", "--oai"}:
        return (parts[1].strip() if len(parts) > 1 else ""), "openai"

    return text, "auto"


def _gemini_sync(query: str) -> dict:
    """Synchronous Gemini call (offloaded to thread)."""
    try:
        if not gemini_client:
            return {"success": False, "error": "Gemini API key not configured", "service": "Gemini"}

        response = gemini_client.models.generate_content(
            model="gemini-2.5-flash",
            contents=query,
        )
        return {"success": True, "text": response.text, "service": "Gemini"}
    except Exception as e:
        error_msg = f"{type(e).__name__}: {str(e)}"
        print(f"Gemini Error: {error_msg}")
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
            temperature=0.7,
        )
        return {"success": True, "text": response.choices[0].message.content, "service": "OpenAI"}
    except Exception as e:
        error_msg = f"{type(e).__name__}: {str(e)}"
        print(f"OpenAI Error: {error_msg}")
        return {"success": False, "error": error_msg, "service": "OpenAI"}


def format_response(result: dict) -> str:
    """Return only the model response without terminal-only metadata."""
    return (result["text"] or "").strip()


async def response_generator(query: str, provider: str = "auto"):
    if not GEMINI_API_KEY and not OPENAI_API_KEY:
        yield "Error: No API keys configured. Set GEMINI_API_KEY or OPENAI_API_KEY in .env"
        return

    if provider == "openai":
        result = await query_openai(query)
        if result["success"]:
            yield format_response(result)
            return
        yield f"Error querying {result['service']}: {result.get('error', 'Unknown error')}"
        return

    if GEMINI_API_KEY:
        result = await query_gemini(query)
    else:
        result = await query_openai(query)

    if result["success"]:
        yield format_response(result)
        return

    if GEMINI_API_KEY and OPENAI_API_KEY:
        result = await query_openai(query)
        if result["success"]:
            yield format_response(result)
            return

    yield f"Error querying {result['service']}: {result.get('error', 'Unknown error')}"


@router.post("/ask")
async def ask(request: Request):
    """Ask Gemini or OpenAI a general question."""
    body = await request.body()
    query = body.decode("utf-8").strip()
    query, provider = parse_provider_override(query)

    if not query:
        return PlainTextResponse("No query provided. Use: curl -d 'your question' localhost:8000/ask")

    return StreamingResponse(response_generator(query, provider=provider), media_type="text/plain")