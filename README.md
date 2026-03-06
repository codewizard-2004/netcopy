# netcopy

Query Google Gemini from any terminal with a single curl command — no API keys needed at the prompt.

---

## Quick Start

### 1. Install dependencies

```bash
pip install -r requirements.txt
```

### 2. Set your API key

```bash
cp .env.example .env
nano .env   # replace placeholder with your real GEMINI_API_KEY
```

### 3. Start the server

```bash
./start.sh          # port 8000
./start.sh 8080     # custom port
```

---

## Usage

```bash
curl localhost:8000/ask -d 'What is the speed of light?'
```

That's it. No headers, no JSON, no flags.

### Also works with JSON if you prefer

```bash
curl localhost:8000/ask -H 'Content-Type: application/json' -d '{"query": "your question"}'
```

### Shell alias for even faster use (add to ~/.bashrc)

```bash
alias ask='curl -s localhost:8000/ask -d'
```

Then just:

```bash
ask 'Explain black holes'
```

---

## Deploying to Render

1. Push to GitHub
2. Go to [render.com](https://render.com) → New → Web Service → connect your repo
3. Render auto-detects `render.yaml`
4. In the **Environment** tab, add `GEMINI_API_KEY` = your real key
5. Deploy → live at `https://netcopy.onrender.com`

Query your live server:

```bash
curl https://netcopy.onrender.com/ask -d 'your question'
```

> **Free tier:** spins down after 15 min of inactivity, ~30s cold start.

---

## Endpoints

| Method | Path      | Description           |
|--------|-----------|-----------------------|
| GET    | `/`       | Status + usage hint   |
| GET    | `/health` | Health ping           |
| POST   | `/ask`    | Send a query          |

---

## Project Structure

```
netcopy/
├── main.py           # FastAPI app
├── requirements.txt  # Python dependencies
├── render.yaml       # Render deployment config
├── start.sh          # Local launcher
├── ask.sh            # Minimal CLI wrapper
├── .env.example      # API key template (safe to commit)
├── .env              # Your real key (never committed)
└── README.md
```

---

## Push to GitHub

```bash
git init
git add .
git commit -m "Initial commit: netcopy"
git branch -M main
git remote add origin https://github.com/YOUR_USERNAME/netcopy.git
git push -u origin main
```
