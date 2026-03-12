import os
from pathlib import Path
from typing import Optional
from fastapi import FastAPI, HTTPException, Request
from fastapi.responses import PlainTextResponse
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
        return PlainTextResponse(content=response.text)
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

    # List of free models to try as fallbacks
    models_to_try = [
        "meta-llama/llama-3.3-70b-instruct:free",
        "google/gemini-2.0-flash-exp:free",
        "mistralai/pixtral-12b:free",
        "openrouter/auto" # Fallback to OpenRouter's auto-router
    ]

    last_error = None
    for model in models_to_try:
        try:
            # Strip potential whitespace from the key
            api_key = OPEN_ROUTER_API_KEY.strip()
            
            async with httpx.AsyncClient() as httpx_client:
                headers = {
                    "Authorization": f"Bearer {api_key}",
                    "HTTP-Referer": "https://netcopy-ai.example.com", 
                    "X-Title": "netcopy-ai",
                    "Content-Type": "application/json",
                }
                
                response = await httpx_client.post(
                    "https://openrouter.ai/api/v1/chat/completions",
                    headers=headers,
                    json={
                        "model": model,
                        "messages": [{"role": "user", "content": query}],
                        "temperature": 0.7,
                    },
                    timeout=60.0,
                )
                
                if response.status_code == 200:
                    data = response.json()
                    answer = data["choices"][0]["message"]["content"]
                    return PlainTextResponse(content=answer)
                
                # If we get here, the specific model failed, try the next one
                last_error = f"Model {model} failed with status {response.status_code}"
                continue

        except Exception as e:
            last_error = str(e)
            continue

    # If all models fail
    raise HTTPException(
        status_code=502, 
        detail=f"All OpenRouter models failed. Last error: {last_error}"
    )
