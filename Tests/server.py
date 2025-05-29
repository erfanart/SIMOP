import os
import asyncio
from aiohttp import web
from vosk import Model, KaldiRecognizer

# مسیر به خروجی بیلد شده Next.js (فولدر "out")
STATIC_DIR = os.path.abspath("./out")
model = Model("/home/erf/vosk-models/vosk-model-small-fa-0.42")

async def websocket_handler(request):
    ws = web.WebSocketResponse()
    await ws.prepare(request)

    recognizer = KaldiRecognizer(model, 16000)
    recognizer.SetWords(True)

    try:
        async for msg in ws:
            if msg.type == web.WSMsgType.BINARY:
                if recognizer.AcceptWaveform(msg.data):
                    await ws.send_str(recognizer.Result())
                else:
                    await ws.send_str(recognizer.PartialResult())
    except Exception as e:
        print(f"WebSocket error: {e}")
    return ws

async def index(request):
    return web.FileResponse(os.path.join(STATIC_DIR, "index.html"))

app = web.Application()
app.router.add_get("/", index)
app.router.add_get("/ws", websocket_handler)
app.router.add_static("/", STATIC_DIR)

if __name__ == "__main__":
    web.run_app(app, port=2700)

