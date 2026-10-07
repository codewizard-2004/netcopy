"""Help and root endpoints."""

from fastapi import APIRouter
from fastapi.responses import PlainTextResponse

router = APIRouter()


def format_home() -> str:
    """Generate a clean landing page for the root endpoint."""
    return """
╔══════════════════════════════════════════════════════════════╗
║                         NETCOPY                              ║
╚══════════════════════════════════════════════════════════════╝

Fast terminal access to Gemini by default, with OpenAI override support.

Try one of these:
  /help      full command reference
  /ask       general questions
  /code      code-only generation
  /files     list downloadable files
  /download  download a file by path
  /health    service status
""".strip() + "\n"


def format_help(topic: str | None = None) -> str:
    """Generate detailed help text for terminal display."""
    general = """
╔══════════════════════════════════════════════════════════════╗
║                         NETCOPY                              ║
╚══════════════════════════════════════════════════════════════╝

Use /help/<thing> to learn more about a specific command.

Available topics:
  /help/ask
  /help/code
  /help/files
  /help/download
  /help/health

Quick examples:
  curl -d 'explain recursion' localhost:8000/ask
  curl -d '-oai write a Python factorial function' localhost:8000/code -o factorial.py
  curl localhost:8000/files
  curl localhost:8000/download/chat_app/client.c -o client.c

What each topic covers:
  ask       General chat and model selection
  code      Code-only generation with language detection
  files     Browsing downloadable files
  download  Downloading files by path, including nested paths
  health    Service health check
""".strip()

    sections = {
        "ask": """
/help/ask

Ask a general question or request an explanation.

Default model behavior:
  - Gemini is tried first
  - OpenAI is used automatically as fallback if Gemini fails

Force OpenAI for a request:
  curl -d '-oai explain how a binary search tree works' localhost:8000/ask

Normal request:
  curl -d 'explain how a binary search tree works' localhost:8000/ask

Save the answer to a file:
  curl -d 'explain how a binary search tree works' localhost:8000/ask -o notes.txt
""".strip(),
        "code": """
/help/code

Generate code only. The service detects the target language from your prompt.

Default model behavior:
  - Gemini is tried first
  - OpenAI is used automatically as fallback if Gemini fails

Force OpenAI for a request:
  curl -d '-oai write a python function for factorial' localhost:8000/code -o factorial.py

Examples:
  curl -d 'write a python function for factorial' localhost:8000/code -o factorial.py
  curl -d 'write a socket server in C for a chat client' localhost:8000/code -o server.c
  curl -d 'create a REST API endpoint that handles JWT authentication' localhost:8000/code -o auth.py

Notes:
  - Explanatory text is converted into language-style comments when needed
  - The response is optimized to be pasted directly into a source file
""".strip(),
        "files": """
/help/files

List downloadable files stored under downloads/.

Example:
  curl localhost:8000/files

What you get:
  - A tree-style listing of files and directories
  - File sizes for downloadable items
  - Only files with extensions are shown

If a file is listed, download it with:
  curl localhost:8000/download/<path> -o <output-file>

Examples:
  curl localhost:8000/download/dvr.c -o dvr.c
  curl localhost:8000/download/chat_app/client.c -o client.c
  curl localhost:8000/download/ftp/networking.txt -o networking.txt
""".strip(),
        "download": """
/help/download

Download a specific file from the downloads/ directory.

Path format:
  curl localhost:8000/download/<relative-path>

Examples:
  curl localhost:8000/download/dvr.c -o dvr.c
  curl localhost:8000/download/chat_app/client.c -o client.c
  curl localhost:8000/download/ftp/networking.txt -o networking.txt

Rules:
  - Use the relative path shown by /files
  - Nested paths are supported
  - Path traversal is blocked for safety
""".strip(),
        "health": """
/help/health

Check whether the service is running.

Example:
  curl localhost:8000/health

Use this when you only want a fast readiness check or a deployment probe.
""".strip(),
    }

    if topic is None:
        return general + "\n"

    return sections.get(topic, general + "\n") + "\n"


@router.get("/", response_class=PlainTextResponse)
async def root():
    """Root endpoint - show the clean home view."""
    return format_home()


@router.get("/help", response_class=PlainTextResponse)
async def help_endpoint():
    """Help endpoint - show detailed command help."""
    return format_help()


@router.get("/help/{topic}", response_class=PlainTextResponse)
async def topic_help(topic: str):
    """Topic-specific help endpoint."""
    return format_help(topic.lower())
