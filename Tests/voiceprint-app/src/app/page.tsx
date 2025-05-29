// src/app/page.tsx
'use client'

import VoiceClient from '@/components/VoiceClient'

export default function Home() {
  return (
    <main className="p-4">
      <h1 className="text-2xl font-bold mb-4">🧠 Voice Assistant</h1>
      <VoiceClient />
    </main>
  )
}

