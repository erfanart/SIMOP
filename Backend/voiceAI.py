import os
import json
import sounddevice as sd
from scipy.io.wavfile import write
from groq import Groq

# Initialize Groq client
client = Groq(api_key="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")

def record_audio(filename="input.wav", duration=5, fs=44100):
    print(f"🎙️ Recording for {duration} seconds...")
    audio = sd.rec(int(duration * fs), samplerate=fs, channels=1)
    sd.wait()
    write(filename, fs, audio)
    print(f"✅ Audio saved to {filename}")

def transcribe_with_groq(filename):
    with open(filename, "rb") as file:
        transcription = client.audio.transcriptions.create(
            file=file,
            model="whisper-large-v3-turbo",
            response_format="verbose_json",  # detailed timestamps etc
            language="fa",
            temperature=0.0
        )
    print("🗣️ Transcription Result:")
    print(json.dumps(transcription, indent=2, default=str))
    # To get plain text:
    print("\nPlain text transcription:\n", transcription.text)

if __name__ == "__main__":
    filename = "input.wav"
    record_audio(filename=filename, duration=5)
    transcribe_with_groq(filename)
