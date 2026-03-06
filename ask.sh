#!/usr/bin/env bash
# ask.sh — Minimal wrapper, prints only the response text
# Usage: ./ask.sh "your question"

SERVER=${GEMINI_SERVER:-"http://localhost:8000"}

if [ -z "$1" ]; then
  echo "Usage: ./ask.sh \"your question\""
  exit 1
fi

curl -s -X POST "$SERVER/ask" -d "$*" \
  | python3 -c "import json,sys; print(json.load(sys.stdin)['response'])"
