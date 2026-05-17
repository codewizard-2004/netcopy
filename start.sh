#!/bin/bash

# netcopy server starter script
# Usage: ./start.sh [port]
# Default port: 8000

PORT=${1:-8000}

echo "Starting netcopy server on port $PORT..."
echo ""
echo "Available endpoints:"
echo "  GET  /          - Show help"
echo "  GET  /help      - Show help"
echo "  POST /ask       - Query Gemini/OpenAI"
echo "  GET  /files     - List files"
echo "  GET  /download/<path> - Download file"
echo "  GET  /health    - Health check"
echo ""
echo "Try: curl -d 'your question' localhost:$PORT/ask"
echo ""

uvicorn main:app --host 0.0.0.0 --port "$PORT" --reload
