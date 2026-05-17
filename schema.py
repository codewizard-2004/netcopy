"""Pydantic models and data schemas for netcopy."""

from pydantic import BaseModel
from typing import Optional


class QueryRequest(BaseModel):
    """Schema for query request."""
    query: str
    language: Optional[str] = "en"


class FileInfo(BaseModel):
    """Schema for file information."""
    filename: str
    size_kb: float
    description: Optional[str] = None
