'use client'
import { useRef, useState } from 'react'

export default function VoiceRecorder() {
  const [text, setText] = useState('')
  const [recording, setRecording] = useState(false)
  const socketRef = useRef<WebSocket | null>(null)
  const processorRef = useRef<ScriptProcessorNode | null>(null)
  const audioCtxRef = useRef<AudioContext | null>(null)
  const streamRef = useRef<MediaStream | null>(null)

  const startRecording = async () => {
    if (!navigator.mediaDevices) {
      alert("Browser does not support media devices.")
      return
    }

    const stream = await navigator.mediaDevices.getUserMedia({ audio: true })
    streamRef.current = stream
    const audioCtx = new AudioContext({ sampleRate: 16000 })
    audioCtxRef.current = audioCtx

    const source = audioCtx.createMediaStreamSource(stream)
    const processor = audioCtx.createScriptProcessor(4096, 1, 1)

    const socket = new WebSocket("ws://localhost:2700/ws")
    socket.binaryType = "arraybuffer"

    socket.onmessage = (e) => {
      const data = JSON.parse(e.data)
      if (data.text) setText(data.text)
    }
    socketRef.current = socket

    processor.onaudioprocess = (e) => {
      const input = e.inputBuffer.getChannelData(0)
      const buf = new Int16Array(input.length)
      for (let i = 0; i < input.length; i++) {
        const s = Math.max(-1, Math.min(1, input[i]))
        buf[i] = s < 0 ? s * 0x8000 : s * 0x7FFF
      }
      if (socket.readyState === WebSocket.OPEN) {
        socket.send(buf.buffer)
      }
    }

    source.connect(processor)
    processor.connect(audioCtx.destination)
    processorRef.current = processor
    setRecording(true)
  }

  const stopRecording = () => {
    processorRef.current?.disconnect()
    streamRef.current?.getTracks().forEach((t) => t.stop())
    audioCtxRef.current?.close()
    socketRef.current?.close()
    setRecording(false)
  }

  return (
    <div className="p-4 space-y-4 border rounded bg-white shadow-md">
      <h2 className="text-xl font-bold">🎤 Voice Recorder</h2>
      <button
        onClick={recording ? stopRecording : startRecording}
        className={`px-4 py-2 rounded font-semibold ${
          recording ? 'bg-red-600 text-white' : 'bg-green-600 text-white'
        }`}
      >
        {recording ? '⏹ Stop Recording' : '🎙 Start Recording'}
      </button>
      <div className="border p-4 bg-gray-100 text-gray-800 rounded whitespace-pre-wrap min-h-[4rem]">
        {text || "⏳ منتظر گفتار..."}
      </div>
    </div>
  )
}

