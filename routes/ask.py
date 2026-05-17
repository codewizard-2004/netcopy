"""Ask endpoint for querying Gemini and OpenAI."""

import os
import asyncio
import sys
from fastapi import APIRouter, Request
from fastapi.responses import PlainTextResponse, StreamingResponse
from google import genai
from openai import AsyncOpenAI

router = APIRouter()

GEMINI_API_KEY = os.getenv("GEMINI_API_KEY")
OPENAI_API_KEY = os.getenv("OPENAI_API_KEY")

gemini_client = genai.Client(api_key=GEMINI_API_KEY) if GEMINI_API_KEY else None
openai_client = AsyncOpenAI(api_key=OPENAI_API_KEY) if OPENAI_API_KEY else None

# Spinner frames for loading animation
SPINNER_FRAMES = ["⠋", "⠙", "⠹", "⠸", "⠼", "⠴", "⠦", "⠧", "⠇", "⠏"]


async def async_spinner():
    """Generator for spinner animation."""
    for frame in SPINNER_FRAMES:
        yield f"\r{frame} Processing..."
        await asyncio.sleep(0.1)


def _gemini_sync(query: str) -> dict:
    """Synchronous Gemini call (offloaded to thread)."""
    try:
        if not gemini_client:
            return {"success": False, "error": "Gemini API key not configured", "service": "Gemini"}

        response = gemini_client.models.generate_content(
            model="gemini-2.0-flash",
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
    backup_note = " (backup)" if is_backup else ""
    return f"{result['text']}\n\n---\n[Powered by {result['service']}{backup_note}]"


async def response_generator(query: str):
    if not GEMINI_API_KEY and not OPENAI_API_KEY:
        yield "Error: No API keys configured. Set GEMINI_API_KEY or OPENAI_API_KEY in .env"
        return

    # Start API call concurrently
    if GEMINI_API_KEY:
        task = asyncio.create_task(query_gemini(query))
    else:
        task = asyncio.create_task(query_openai(query))

    # Spin while the task is running
    frame_idx = 0
    while not task.done():
        frame = SPINNER_FRAMES[frame_idx % len(SPINNER_FRAMES)]
        yield f"\r{frame} Processing...   "
        frame_idx += 1
        await asyncio.sleep(0.1)

    result = await task

    # Clear spinner line
    yield "\r" + " " * 30 + "\r"

    if result["success"]:
        yield format_response(result, is_backup=False)
        return

    # Gemini failed, try OpenAI fallback
    if GEMINI_API_KEY and OPENAI_API_KEY:
        yield "Gemini failed, trying OpenAI...\n"
        result = await query_openai(query)
        if result["success"]:
            yield format_response(result, is_backup=True)
            return
        yield f"Error querying {result['service']}: {result.get('error', 'Unknown error')}"
        return

    yield f"Error querying {result['service']}: {result.get('error', 'Unknown error')}"


@router.post("/ask")
async def ask(request: Request):
    """
    Ask Gemini or OpenAI a question with streaming response and loader.

    Query with: curl -d 'your question' localhost:8000/ask
    Save to file: curl -d 'your question' localhost:8000/ask -o answer.txt
    """
    body = await request.body()
    query = body.decode("utf-8").strip()

    if not query:
        return PlainTextResponse("No query provided. Use: curl -d 'your question' localhost:8000/ask")

    return StreamingResponse(response_generator(query), media_type="text/plain")