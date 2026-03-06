import os
from pathlib import Path
from fastapi import FastAPI, HTTPException, Request
from pydantic import BaseModel
from dotenv import load_dotenv
import google.generativeai as genai

# Load API key from .env file (falls back to actual env vars if not in .env)
load_dotenv(dotenv_path=Path(__file__).parent / ".env")

GEMINI_API_KEY = os.environ.get("GEMINI_API_KEY")
if not GEMINI_API_KEY:
    raise RuntimeError(
        "GEMINI_API_KEY is not set. Add it to your .env file:\n"
        "  echo 'GEMINI_API_KEY=your_key_here' > .env"
    )

genai.configure(api_key=GEMINI_API_KEY)
model = genai.GenerativeModel("gemini-2.0-flash")

app = FastAPI(
    title="netcopy",
    description="Query Google Gemini from the terminal: curl localhost:8000/ask -d 'your question'",
    version="1.0.0",
)


class QueryResponse(BaseModel):
    response: str
    model: str


@app.get("/")
def root():
    return {"status": "running", "usage": "curl localhost:8000/ask -d 'your question'"}


@app.get("/health")
def health():
    return {"status": "ok"}


@app.post("/ask")
async def ask(request: Request):
    content_type = request.headers.get("content-type", "")

    if "application/json" in content_type:
        body = await request.json()
        query = body.get("query") or body.get("q") or ""
    else:
        # Plain text body — default when using curl -d 'text'
        raw = await request.body()
        query = raw.decode("utf-8").strip()

    if not query:
        raise HTTPException(
            status_code=400,
            detail="Query cannot be empty. Usage: curl localhost:8000/ask -d 'your question'"
        )

    try:
        result = model.generate_content(query)
        return QueryResponse(response=result.text, model="gemini-2.0-flash")
    except Exception as e:
        raise HTTPException(status_code=500, detail=f"Gemini API error: {str(e)}")
