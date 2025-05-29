#ifndef MOTOR_H
#define MOTOR_H

#include <Arduino.h>

class Motor
{
public:
    Motor(uint8_t mainpin, uint8_t directionpin);
    void begin();
    void setState(bool on);
    void setDirection(bool on);

private:
    uint8_t _directionpin;
    uint8_t _mainpin;
};

#endif
