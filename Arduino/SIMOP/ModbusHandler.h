#ifndef MODBUSHANDLER_H
#define MODBUSHANDLER_H

#include <ModbusRTU.h>
#include "Motor.h"
#include "Encoder.h"
#include "DHTSensor.h"

class ModbusHandler
{
public:
    ModbusHandler(ModbusRTU *mb, Motor *motor, Encoder *encoder, DHTSensor *dht);
    void begin();
    void task();

private:
    static uint16_t cbWriteMotor(TRegister *reg, uint16_t len);

    ModbusRTU *mb;
    Motor *motor;
    Encoder *encoder;
    DHTSensor *dht;

    float lastTemp;
    float lastHum;
    unsigned long lastRPMUpdate;

    // Instance pointer for static callback access
    static ModbusHandler *instance;

    // Modbus registers
    static const uint16_t HREG_TEMP = 100;
    static const uint16_t HREG_HUM = 101;
    static const uint16_t HREG_MOTOR = 102;
    static const uint16_t HREG_RPM = 103;
};

#endif
