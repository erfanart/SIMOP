#include "Motor.h"

Motor::Motor(uint8_t mainpin, uint8_t directionpin) : _mainpin(mainpin), _directionpin(directionpin) {}

void Motor::begin()
{
    pinMode(_mainpin, OUTPUT);
    digitalWrite(_mainpin, HIGH);
    pinMode(_directionpin, OUTPUT);
    digitalWrite(_directionpin, HIGH);
}

void Motor::setState(bool on)
{
    digitalWrite(_mainpin, on ? LOW : HIGH);
}

void Motor::setDirection(bool on)
{
    digitalWrite(_directionpin, on ? LOW : HIGH);
}
