# netcopy

Query Google Gemini by default, or force OpenAI with a simple `-oai` flag. No API keys needed at the prompt. Download files effortlessly. Perfect for code generation and retrieval.

---

## Quick Start

### 1. Install dependencies

```bash
uv sync
```

### 2. Set your API keys

```bash
cp .env.example .env
nano .env   # Add your GEMINI_API_KEY and/or OPENAI_API_KEY
```

### 3. Start the server

```bash
uvicorn main:app --host 0.0.0.0 --port 8000
```

Or use the provided script:
```bash
./start.sh          # port 8000
./start.sh 8080     # custom port
```

---

## Features

✨ **Gemini + OpenAI Integration** - Tries Gemini first, falls back to OpenAI automatically  
📝 **Code Generation** - Optimized for generating code snippets  
📁 **File Management** - Browse and download files with nested directory support  
⚡ **Streaming Responses** - Real-time response streaming with animated loader  
🛡️ **Secure** - Path traversal protection for file downloads  
🔌 **Modular Architecture** - Clean route separation with Pydantic schemas  

---

## Usage

### Get Help
```bash
curl localhost:8000/help
curl localhost:8000/help/ask
curl localhost:8000/help/code
curl localhost:8000/help/files
curl localhost:8000/help/download
curl localhost:8000/help/health
curl localhost:8000/        # Clean home landing page
```

### Ask a Question (General Chat)
```bash
curl -d 'explain how a binary search tree works' localhost:8000/ask
curl -d '-oai explain how a binary search tree works' localhost:8000/ask
```

**Save response to file:**
```bash
curl -d 'explain how a binary search tree works' localhost:8000/ask -o notes.txt
```

### Generate Code Only
```bash
curl -d 'write a python function for factorial' localhost:8000/code -o code.py
curl -d 'write a socket server in C for a chat client' localhost:8000/code -o server.c
curl -d '-oai write a python function for factorial' localhost:8000/code -o code.py
```

The `/code` endpoint only returns code output and keeps any explanatory text as comments in the requested language. Gemini is used first by default, then OpenAI as fallback unless you force OpenAI with `-oai`.

**With longer queries:**
```bash
curl -d 'create a REST API endpoint that handles user authentication with JWT tokens' localhost:8000/code -o auth.py
```

### List Available Files
```bash
curl localhost:8000/files
```

Output shows a tree structure:
```
📁 Available Files & Directories:
============================================================

├── dvr.c (5.2 KB)
├── index.html (3.1 KB)
├── leaky.c (2.8 KB)
├── chat_app/
│   ├── client.c (4.5 KB)
│   └── server.c (6.2 KB)
├── ftp/
│   ├── client.c (3.9 KB)
│   ├── networking.txt (1.2 KB)
│   └── physics.txt (2.1 KB)
└── tcp/
    ├── client.c (3.5 KB)
    └── server.c (5.8 KB)

============================================================
Download a file: curl localhost:8000/download/<path>
Example: curl localhost:8000/download/chat_app/client.c -o client.c
```

### Download Files (including nested files)
```bash
# Download from root
curl localhost:8000/download/dvr.c -o dvr.c

# Download from subdirectory
curl localhost:8000/download/chat_app/client.c -o client.c

# Download deeply nested files
curl localhost:8000/download/ftp/networking.txt -o networking.txt
```

### Health Check
```bash
curl localhost:8000/health
```

---

## API Endpoints

| Method | Path               | Description                                |
|--------|-------------------|--------------------------------------------|
| GET    | `/`               | Show help                                  |
| GET    | `/help`           | Show detailed help                         |
| GET    | `/help/ask`       | Ask endpoint usage and examples            |
| GET    | `/help/code`      | Code endpoint usage and examples           |
| GET    | `/help/files`     | File listing usage and examples            |
| GET    | `/help/download`  | Download usage and examples                |
| POST   | `/ask`            | Query Gemini/OpenAI                        |
| POST   | `/code`           | Generate code-only output in the target language |
| GET    | `/files`          | List available files                       |
| GET    | `/download/<path>`| Download specific file                     |
| GET    | `/health`         | Health check                               |

---

## Shell Alias (Optional)

Add to `~/.bashrc` or `~/.zshrc`:

```bash
alias ask='curl -s localhost:8000/ask -d'
```

Then use directly:

```bash
ask 'write a fibonacci function'
ask 'explain binary search algorithm'
```

---

## Examples

### Generate Python Code
```bash
curl -d 'write a decorator that caches function results' localhost:8000/ask -o cache_decorator.py
```

### Generate C Code with Files
```bash
curl -d 'write a socket-based chat server in C' localhost:8000/ask -o server.c
```

### Get Configuration Code
```bash
curl -d 'fastapi cors middleware configuration' localhost:8000/ask
```

### Force OpenAI for a Request
```bash
curl -d '-oai write a python function that reverses a string' localhost:8000/ask
curl -d '-oai write a python function for factorial' localhost:8000/code -o factorial.py
```

### Download Existing Code and Generate Related Code
```bash
# Download existing file
curl localhost:8000/download/chat_app/client.c -o client.c

# Generate compatible server code
curl -d 'write a corresponding server for this chat client in C' localhost:8000/ask -o server.c
```

---

## How It Works

### Query Processing
1. **Request** → curl sends your question
2. **Spinner** → Animated loader while processing
3. **Gemini** → Tries Google Gemini API first
4. **Fallback** → If Gemini fails, automatically tries OpenAI
5. **Response** → Streams result with service info

### Example Response
```
def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)

---
[Powered by Gemini]
```

If OpenAI was used as backup:
```
...response...

---
[Powered by OpenAI (backup)]
```

---

## Environment Setup

### `.env` File
```env
GEMINI_API_KEY=your_gemini_api_key_here
OPENAI_API_KEY=your_openai_api_key_here
```

- **GEMINI_API_KEY**: Get from [Google AI Studio](https://aistudio.google.com/app/apikey)
- **OPENAI_API_KEY**: Get from [OpenAI Platform](https://platform.openai.com/api-keys)

At least one API key is required. Both are recommended for reliability.

---

## Project Structure

```
netcopy/
├── main.py                 # FastAPI app initialization
├── schema.py              # Pydantic models
├── routes/                # API endpoints
│   ├── __init__.py
│   ├── help.py           # Help endpoint
│   ├── ask.py            # Gemini/OpenAI query endpoint
│   ├── files.py          # File listing & download
│   └── health.py         # Health check
├── downloads/            # Files available for download
├── pyproject.toml        # Project dependencies
├── render.yaml           # Render deployment config
├── .env.example          # API key template
├── .env                  # Your API keys (never commit)
└── README.md
```

---

## Deployment to Render

1. **Push to GitHub**
   ```bash
   git init
   git add .
   git commit -m "Initial commit: netcopy"
   git branch -M main
   git remote add origin https://github.com/YOUR_USERNAME/netcopy.git
   git push -u origin main
   ```

2. **Deploy on Render**
   - Go to [render.com](https://render.com)
   - Click "New" → "Web Service"
   - Connect your GitHub repository
   - Render auto-detects `render.yaml`

3. **Configure Environment**
   - In Render dashboard, go to "Environment"
   - Add `GEMINI_API_KEY` = your API key
   - Add `OPENAI_API_KEY` = your API key (optional)
   - Deploy

4. **Access Your Server**
   ```bash
   curl https://your-service-name.onrender.com/ask -d 'your question'
   curl https://your-service-name.onrender.com/files
   curl https://your-service-name.onrender.com/download/filename.c
   ```

> **Note:** Free tier spins down after 15 min of inactivity (~30s cold start)

---

## Dependencies

- **fastapi** - Web framework
- **uvicorn** - ASGI server
- **google-genai** - Google Gemini API
- **openai** - OpenAI API
- **python-dotenv** - Environment variables
- **httpx** - HTTP client

---

## Troubleshooting

### No API response
- Check `.env` file has valid API keys
- Verify internet connection
- Check server logs for errors

### Spinner not visible
- Ensure curl is not using `-s` flag (which silences output)
- Use: `curl localhost:8000/ask -d 'question'` (not `-s`)

### Files not showing
- Verify `downloads/` directory exists
- Only files with extensions are listed (`.c`, `.txt`, `.html`, etc.)
- Binary files without extensions are hidden

### Download fails
- Check file path is correct
- Use subdirectory format: `chat_app/client.c`
- File must exist in `downloads/` directory

---

## Tips & Tricks

### Generate Multiple Related Files
```bash
# Generate main function
curl -d 'write a main function that uses socket programming' localhost:8000/ask -o main.c

# Generate server
curl -d 'write a socket server for main.c to connect to' localhost:8000/ask -o server.c

# Generate header
curl -d 'write header file definitions for server-client communication' localhost:8000/ask -o protocol.h
```

### Batch Download Files
```bash
# Download multiple files
curl localhost:8000/download/chat_app/client.c -o client.c
curl localhost:8000/download/chat_app/server.c -o server.c
curl localhost:8000/download/tcp/client.c -o tcp_client.c
```

### Use with Shell Scripts
```bash
#!/bin/bash
# Generate and save code
ask 'write a shell script for backup' > backup.sh
chmod +x backup.sh

# Download and use
curl localhost:8000/download/ftp/client.c -o ftp_client.c
```

---

## Made with ❤️ for the terminal
