"""File listing and download endpoints."""

import os
from pathlib import Path
from fastapi import APIRouter
from fastapi.responses import PlainTextResponse, FileResponse

router = APIRouter()


def get_directory_tree(path: str, prefix: str = "", is_last: bool = True) -> str:
    """Generate a tree-like representation of directory structure (files with extensions only)."""
    output = ""
    
    if not os.path.exists(path):
        return "Directory not found."
    
    try:
        items = sorted(os.listdir(path))
    except PermissionError:
        return "Permission denied."
    
    if not items:
        return "Empty directory."
    
    for i, item in enumerate(items):
        item_path = os.path.join(path, item)
        is_last_item = i == len(items) - 1
        
        # Determine the connector characters for the tree
        connector = "└── " if is_last_item else "├── "
        extension = "    " if is_last_item else "│   "
        
        if os.path.isdir(item_path):
            # Directory
            output += f"{prefix}{connector}{item}/\n"
            sub_items = sorted(os.listdir(item_path))
            sub_items = [s for s in sub_items if os.path.isfile(os.path.join(item_path, s)) and '.' in s][:5]  # Only files with extensions
            
            for j, sub_item in enumerate(sub_items):
                sub_path = os.path.join(item_path, sub_item)
                is_last_sub = j == len(sub_items) - 1
                sub_connector = "└── " if is_last_sub else "├── "
                
                size = os.path.getsize(sub_path) / 1024
                output += f"{prefix}{extension}{sub_connector}{sub_item} ({size:.1f} KB)\n"
            
            # Show if there are more items
            remaining = len([s for s in os.listdir(item_path) if os.path.isfile(os.path.join(item_path, s)) and '.' in s])
            if remaining > 5:
                output += f"{prefix}{extension}└── ... and {remaining - 5} more files\n"
        else:
            # Only show files with extensions (not binary files)
            if '.' in item:
                size = os.path.getsize(item_path) / 1024
                output += f"{prefix}{connector}{item} ({size:.1f} KB)\n"
    
    return output


def collect_all_files(path: str, base_path: str = "") -> list:
    """Recursively collect all files with extensions only (no binary files)."""
    files = []
    
    try:
        for item in sorted(os.listdir(path)):
            item_path = os.path.join(path, item)
            relative_path = os.path.join(base_path, item) if base_path else item
            
            if os.path.isdir(item_path):
                # Recursively get files from subdirectory
                files.extend(collect_all_files(item_path, relative_path))
            else:
                # Only include files with extensions (skip binary files)
                if '.' in item:
                    size = os.path.getsize(item_path) / 1024
                    files.append((relative_path, size))
    except PermissionError:
        pass
    
    return files


@router.get("/files", response_class=PlainTextResponse)
async def list_files():
    """List available files and directories for download."""
    files_dir = "downloads"
    
    if not os.path.exists(files_dir):
        return "No files directory found."
    
    try:
        items = os.listdir(files_dir)
    except PermissionError:
        return "Permission denied."
    
    if not items:
        return "No files available for download."
    
    output = "📁 Available Files & Directories:\n"
    output += "=" * 60 + "\n\n"
    output += get_directory_tree(files_dir)
    output += "\n" + "=" * 60 + "\n"
    output += "\nDownload a file: curl localhost:8000/download/<path>\n"
    output += "Example: curl localhost:8000/download/chat_app/client.c -o client.c\n"
    
    return output


@router.get("/download/{file_path:path}")
async def download(file_path: str):
    """Download a specific file (supports nested paths)."""
    files_dir = "downloads"
    
    # Normalize and secure the path
    full_path = os.path.normpath(os.path.join(files_dir, file_path))
    
    # Security check: ensure the path is within downloads directory
    if not full_path.startswith(os.path.normpath(files_dir)):
        return PlainTextResponse(
            "Error: Invalid path",
            status_code=400
        )
    
    if not os.path.exists(full_path):
        return PlainTextResponse(
            f"File not found: {file_path}",
            status_code=404
        )
    
    if os.path.isdir(full_path):
        return PlainTextResponse(
            f"Error: {file_path} is a directory, not a file",
            status_code=400
        )
    
    try:
        return FileResponse(full_path, filename=os.path.basename(file_path))
    except PermissionError:
        return PlainTextResponse(
            "Error: Permission denied",
            status_code=403
        )
