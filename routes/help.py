"""Help and root endpoints."""

from fastapi import APIRouter
from fastapi.responses import PlainTextResponse

router = APIRouter()


def format_help() -> str:
    """Generate beautifully formatted help text for terminal display."""
    help_text = """
╔══════════════════════════════════════════════════════════════╗
║                         NETCOPY                              ║
╚══════════════════════════════════════════════════════════════╝

📋 QUICK USAGE
─────────────────────────────────────────────────────────────────

Ask a question:
  curl -d 'your question' <url>/ask

Save output to file:
  curl -d 'your question' <url>/ask -o answer.txt

─────────────────────────────────────────────────────────────────
🔗 AVAILABLE ENDPOINTS
─────────────────────────────────────────────────────────────────

  /ask              Query 
  /files            List downloadable files
  /download/<file>  Download a specific file
  /health           Health check
  /help             Show this message

─────────────────────────────────────────────────────────────────
"""
    return help_text


@router.get("/", response_class=PlainTextResponse)
async def root():
    """Root endpoint - show help."""
    return format_help()


@router.get("/help", response_class=PlainTextResponse)
async def help_endpoint():
    """Help endpoint - show beautiful terminal help."""
    return format_help()
