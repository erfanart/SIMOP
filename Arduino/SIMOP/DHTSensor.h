#ifndef DHTSENSOR_H
#define DHTSENSOR_H

#include <DHT.h>

class DHTSensor
{
public:
    DHTSensor(uint8_t pin, uint8_t type);
    void begin();
    float readTemperature();
    float readHumidity();

private:
    DHT dht;
    float temperature;
    float humidity;
};

#endif
