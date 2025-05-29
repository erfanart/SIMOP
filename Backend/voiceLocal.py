import sounddevice as sd
import queue
import sys
import json
from vosk import Model, KaldiRecognizer

# ================== Configuration ==================
MODEL_PATH = "/home/erf/vosk-models/vosk-model-small-fa-0.42"
SAMPLE_RATE = 16000
BLOCK_SIZE = 8000
CHANNELS = 1

# ================== Initialize =====================
q = queue.Queue()

def audio_callback(indata, frames, time, status):
    if status:
        print("⚠️ Audio status:", status, file=sys.stderr)
    q.put(bytes(indata))

try:
    # Load Vosk model
    print("📦 Loading model...")
    model = Model(MODEL_PATH)
    recognizer = KaldiRecognizer(model, SAMPLE_RATE)
    print("✅ Model loaded successfully.")

    # Open microphone stream
    with sd.RawInputStream(
        samplerate=SAMPLE_RATE,
        blocksize=BLOCK_SIZE,
        dtype='int16',
        channels=CHANNELS,
        callback=audio_callback
    ):
        print("🎤 Listening... (press Ctrl+C to stop)")
        while True:
            data = q.get()
            if recognizer.AcceptWaveform(data):
                result = json.loads(recognizer.Result())
                print("🗣️ You said:", result["text"])
            else:
                partial = json.loads(recognizer.PartialResult())
                if partial["partial"]:
                    print("⌛ Heard:", partial["partial"], end='\r')

except KeyboardInterrupt:
    print("\n👋 Exiting gracefully...")
except Exception as e:
    print("❌ Error:", e)
