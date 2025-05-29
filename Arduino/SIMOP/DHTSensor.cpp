#include "DHTSensor.h"

DHTSensor::DHTSensor(uint8_t pin, uint8_t type) : dht(pin, type), temperature(NAN), humidity(NAN) {}

void DHTSensor::begin()
{
    dht.begin();
}

float DHTSensor::readTemperature()
{
    temperature = dht.readTemperature();
    return temperature;
}

float DHTSensor::readHumidity()
{
    humidity = dht.readHumidity();
    return humidity;
}
