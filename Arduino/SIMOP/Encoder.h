#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

class Encoder
{
public:
    Encoder(uint8_t pinA, uint8_t pinB, uint16_t pulsesPerRev = 100, uint8_t quadratureFactor = 2);

    void begin();

    // Call regularly (e.g. in loop), returns RPM based on internal timing
    float getRPM();

private:
    uint8_t encoderA;
    uint8_t encoderB;
    uint16_t pulsesPerRev;
    uint8_t quadratureFactor;
    volatile long pulseCount;

    unsigned long lastRPMTime;
    unsigned long intervalMs;

    static Encoder *instance; // Static instance pointer for ISR

    static void IRAM_ATTR isr();      // Static ISR wrapper
    void IRAM_ATTR handleInterrupt(); // Actual ISR handler
};

#endif
