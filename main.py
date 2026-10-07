from fastapi import FastAPI
from dotenv import load_dotenv

# Import routers
from routes import help, ask, code, files, health

load_dotenv()

app = FastAPI(title="netcopy", version="0.1.0")

# Register routers
app.include_router(help.router)
app.include_router(ask.router)
app.include_router(code.router)
app.include_router(files.router)
app.include_router(health.router)


if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
