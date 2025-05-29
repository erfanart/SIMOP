#include "ModbusHandler.h"
#include <Arduino.h>

// Initialize static instance pointer
ModbusHandler *ModbusHandler::instance = nullptr;

ModbusHandler::ModbusHandler(ModbusRTU *mb, Motor *motor, Encoder *encoder, DHTSensor *dht)
    : mb(mb), motor(motor), encoder(encoder), dht(dht), lastTemp(NAN), lastHum(NAN), lastRPMUpdate(0) {}

void ModbusHandler::begin()
{
    // Set static instance pointer for callback
    instance = this;

    mb->begin(&Serial);
    mb->slave(1);

    mb->addHreg(HREG_TEMP);
    mb->addHreg(HREG_HUM);
    mb->addHreg(HREG_MOTOR, 0);
    mb->addHreg(HREG_RPM);

    mb->onSetHreg(HREG_MOTOR, cbWriteMotor);
}

void ModbusHandler::task()
{
    mb->task();

    float temp = dht->readTemperature();
    float hum = dht->readHumidity();

    if (!isnan(temp) && temp != lastTemp)
    {
        lastTemp = temp;
        mb->Hreg(HREG_TEMP, (uint16_t)(temp * 10));
        Serial.printf("Temperature: %.1f°C\n", temp);
    }

    if (!isnan(hum) && hum != lastHum)
    {
        lastHum = hum;
        mb->Hreg(HREG_HUM, (uint16_t)(hum * 10));
        Serial.printf("Humidity: %.1f%%\n", hum);
    }

    if (millis() - lastRPMUpdate >= 1000)
    {
        float rpmVal = encoder->getRPM();
        mb->Hreg(HREG_RPM, (uint16_t)rpmVal);
        Serial.printf("RPM: %.2f\n", rpmVal);
        lastRPMUpdate = millis();
    }
}

uint16_t ModbusHandler::cbWriteMotor(TRegister *reg, uint16_t len)
{
    if (instance && instance->motor)
    {
        switch (reg->value)
        {
        case 0:
            instance->motor->setState(false);
            Serial.println("Motor set to: OFF");
            break;
        case 1:
            instance->motor->setState(true);
            Serial.println("Motor set to: ON");
            break;
        case 2:
            instance->motor->setDirection(false);
            Serial.println("Motor direction set to: REVERSE");
            break;
        case 3:
            instance->motor->setDirection(true);
            Serial.println("Motor direction set to: FORWARD");
            break;
        default:
            Serial.printf("Unknown command: %d\n", reg->value);
            break;
        }
    }
    return len;
}
