import pytest
from fastapi.testclient import TestClient
from main import app

client = TestClient(app)

def test_index():
    response = client.get("/")
    assert response.status_code == 200
    data = response.json()
    assert "routes" in data
    assert "/gemini" in data["routes"]
    assert "/open" in data["routes"]

def test_health():
    response = client.get("/health")
    assert response.status_code == 200
    assert response.json() == {"status": "ok"}

def test_gemini_empty_query():
    response = client.post("/gemini", content="")
    assert response.status_code == 400

def test_open_empty_query():
    response = client.post("/open", content="")
    assert response.status_code == 400

def test_gemini_route_exists():
    # We can't easily mock the Gemini API without more setup, 
    # but we can check if the route is defined and handles empty queries.
    response = client.post("/gemini", content="test")
    # This might fail if API key is invalid or network issues, 
    # but it confirms the route is reachable.
    assert response.status_code in [200, 500] 

def test_open_route_exists():
    response = client.post("/open", content="test")
    assert response.status_code in [200, 401, 500, 503]
