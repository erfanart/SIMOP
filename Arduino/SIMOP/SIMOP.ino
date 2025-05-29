#include <Arduino.h>
#include "Encoder.h"
#include "DHTSensor.h"
#include "Motor.h"
#include "ModbusHandler.h"
#include <ModbusRTU.h>

#define ENCODER_PIN_A 12
#define ENCODER_PIN_B 14
#define PULSES_PER_REV 100

#define DHT_PIN 5
#define DHT_TYPE DHT22

#define MOTOR_PIN 4
#define DIRECTION_PIN 2

ModbusRTU mb;
Motor motor(MOTOR_PIN, DIRECTION_PIN);
Encoder encoder(ENCODER_PIN_A, ENCODER_PIN_B, PULSES_PER_REV);
DHTSensor dht(DHT_PIN, DHT_TYPE);
ModbusHandler modbusHandler(&mb, &motor, &encoder, &dht);

void setup()
{
    Serial.begin(115200);
    delay(1000);
    Serial.println("\nStarting system...\n");

    motor.begin();
    encoder.begin();
    dht.begin();
    modbusHandler.begin();
}

void loop()
{
    modbusHandler.task();
    delay(1); // yield for other tasks
}
