#!/usr/bin/env bash
# start.sh — Launch netcopy
# Usage:
#   cp .env.example .env    # first time only
#   nano .env               # add your GEMINI_API_KEY
#   ./start.sh
#   ./start.sh 8080         # optional custom port

PORT=${1:-8000}

if [ ! -f ".env" ]; then
  echo "❌  .env file not found."
  echo "    Run:  cp .env.example .env  then add your GEMINI_API_KEY"
  exit 1
fi

if grep -q "your_gemini_api_key_here" .env; then
  echo "❌  GEMINI_API_KEY is not set in .env"
  echo "    Edit .env and replace 'your_gemini_api_key_here' with your real key"
  exit 1
fi

echo "🚀  netcopy running at http://localhost:$PORT"
echo "    Usage: curl localhost:$PORT/ask -d 'your question'"
echo ""

uvicorn main:app --host 0.0.0.0 --port "$PORT" --reload
