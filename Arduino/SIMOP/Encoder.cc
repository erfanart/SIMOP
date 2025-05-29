#include "Encoder.h"

Encoder *Encoder::instance = nullptr;

Encoder::Encoder(uint8_t pinA, uint8_t pinB, uint16_t pulsesPerRev, uint8_t quadratureFactor)
    : encoderA(pinA), encoderB(pinB), pulsesPerRev(pulsesPerRev), quadratureFactor(quadratureFactor), pulseCount(0),
      lastRPMTime(0), intervalMs(1000)
{ // default 1 sec interval
    instance = this;
}

void Encoder::begin()
{
    pinMode(encoderA, INPUT_PULLUP);
    pinMode(encoderB, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(encoderA), isr, RISING);
}

float Encoder::getRPM()
{
    unsigned long now = millis();
    if (now - lastRPMTime >= intervalMs)
    {
        lastRPMTime = now;

        noInterrupts();
        long count = pulseCount;
        pulseCount = 0;
        interrupts();

        // Calculate RPM
        float rpm = (count * 60000.0) / (pulsesPerRev * quadratureFactor * intervalMs);
        return rpm;
    }
    return -1.0; // Return -1 if not enough time passed yet
}

void IRAM_ATTR Encoder::isr()
{
    if (instance != nullptr)
    {
        instance->handleInterrupt();
    }
}

void IRAM_ATTR Encoder::handleInterrupt()
{
    bool B = digitalRead(encoderB);
    if (B)
    {
        pulseCount--;
    }
    else
    {
        pulseCount++;
    }
}
