import os
from pathlib import Path
from typing import Optional
from fastapi import FastAPI, HTTPException, Request
from pydantic import BaseModel
from dotenv import load_dotenv
from google import genai
import httpx

# Load API key from .env file (falls back to actual env vars if not in .env)
load_dotenv(dotenv_path=Path(__file__).parent / ".env")

GEMINI_API_KEY = os.environ.get("GEMINI_API_KEY")
OPEN_ROUTER_API_KEY = os.environ.get("OPEN_ROUTER_API_KEY")

if not GEMINI_API_KEY:
    raise RuntimeError(
        "GEMINI_API_KEY is not set. Add it to your .env file:\n"
        "  echo 'GEMINI_API_KEY=your_key_here' > .env"
    )

if not OPEN_ROUTER_API_KEY:
    print("WARNING: OPEN_ROUTER_API_KEY is not set. /open route will not work.")

# Gemini client
client = genai.Client(api_key=GEMINI_API_KEY)
GEMINI_MODEL = "gemini-2.0-flash" # Updated to latest stable flash

# OpenRouter config
OPEN_ROUTER_MODEL = "meta-llama/llama-3.3-70b-instruct:free"

app = FastAPI(
    title="netcopy-ai",
    description=(
        "AI Coding Assistant Server. Available endpoints:\n"
        "- POST /gemini: Query Gemini\n"
        "- POST /open  : Query OpenRouter (free model)\n"
        "- curl -d 'your question' <endpoint>"
    ),
    version="1.1.0",
)


class QueryResponse(BaseModel):
    response: str
    model: str


async def get_query_from_request(request: Request) -> str:
    content_type = request.headers.get("content-type", "")
    if "application/json" in content_type:
        body = await request.json()
        query = body.get("query") or body.get("q") or ""
    else:
        # Plain text body — default when using curl -d 'text'
        raw = await request.body()
        query = raw.decode("utf-8").strip()
    return query


@app.get("/")
def index():
    return {
        "message": "Welcome to netcopy-ai server",
        "purpose": "Ask coding related questions via terminal",
        "routes": {
            "/gemini": "POST - Query Google Gemini (gemini-2.0-flash)",
            "/open": "POST - Query OpenRouter (llama-3.3-70b-instruct:free)",
            "/health": "GET - API health check"
        },
        "usage": "curl -d 'your question' <url>/gemini"
    }


@app.get("/health")
def health():
    return {"status": "ok"}


@app.post("/gemini")
async def ask_gemini(request: Request):
    query = await get_query_from_request(request)

    if not query:
        raise HTTPException(
            status_code=400,
            detail="Query cannot be empty. Usage: curl /gemini -d 'your question'"
        )

    try:
        response = client.models.generate_content(model=GEMINI_MODEL, contents=query)
        return QueryResponse(response=response.text, model=GEMINI_MODEL)
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Gemini API error: {str(e)}")


@app.post("/open")
async def ask_open(request: Request):
    if not OPEN_ROUTER_API_KEY:
        raise HTTPException(status_code=503, detail="OpenRouter API key not configured")

    query = await get_query_from_request(request)

    if not query:
        raise HTTPException(
            status_code=400,
            detail="Query cannot be empty. Usage: curl /open -d 'your question'"
        )

    try:
        async with httpx.AsyncClient() as httpx_client:
            response = await httpx_client.post(
                "https://openrouter.ai/api/v1/chat/completions",
                headers={
                    "Authorization": f"Bearer {OPEN_ROUTER_API_KEY}",
                    "HTTP-Referer": "https://netcopy-ai.example.com", # Required by OpenRouter
                    "X-Title": "netcopy-ai",
                },
                json={
                    "model": OPEN_ROUTER_MODEL,
                    "messages": [{"role": "user", "content": query}],
                    "temperature": 0.7,
                },
                timeout=60.0,
            )
            response.raise_for_status()
            data = response.json()
            answer = data["choices"][0]["message"]["content"]
            return QueryResponse(response=answer, model=OPEN_ROUTER_MODEL)

    except httpx.HTTPStatusError as e:
        detail = "OpenRouter API error"
        try:
            error_data = e.response.json()
            detail = error_data.get("error", {}).get("message", detail)
        except:
            pass
        raise HTTPException(status_code=e.response.status_code, detail=detail)
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"OpenRouter integration error: {str(e)}")
