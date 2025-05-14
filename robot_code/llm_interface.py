import requests
from config import API_KEY, MODEL

def call_llm(messages, title="LLM Chat"):
    headers = {
        "Authorization": f"Bearer {API_KEY}",
        "HTTP-Referer": "http://localhost",
        "X-Title": title
    }

    payload = {
        "model": MODEL,
        "messages": messages,
        "n": 1
    }

    try:
        response = requests.post(
            "https://openrouter.ai/api/v1/chat/completions",
            headers=headers,
            json=payload
        )
        response.raise_for_status()
        return response.json()["choices"][0]["message"]["content"]
    except Exception as e:
        print("❌ LLM request failed:", e)
        return None
