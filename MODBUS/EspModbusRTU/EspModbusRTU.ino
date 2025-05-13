#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <ModbusRTU.h> // Modbus RTU library
#include <DHT.h>
float H = NAN;
float T = NAN;
// DHT11 setup
#define DPIN 14     // Pin to connect DHT sensor (GPIO number) D2
#define DTYPE DHT11 // Define DHT 11 or DHT22 sensor type
DHT dht(DPIN, DTYPE);

#define RX_PIN 13
#define TX_PIN 15
// Modbus RTU configuration
#define SLAVE_ID 1 // Modbus slave ID
#define BAUD_RATE 115200
// For ESP8266, we use the hardware serial pins (GPIO1-TX, GPIO3-RX)
// If you need to use different pins, you'll need SoftwareSerial

SoftwareSerial MbSerial(RX_PIN, TX_PIN);
// Modbus RTU object
ModbusRTU mb;

// Holding register addresses
const int TEMP_REG = 100; // Temperature register
const int HUM_REG = 101;  // Humidity register

void setup()
{
  Serial.begin(115200);
  Serial.println("Initializing Modbus RTU...");

  // Initialize DHT sensor
  dht.begin();

  // Initialize Serial for Modbus RTU (using hardware serial)
  MbSerial.begin(BAUD_RATE); // Uses default GPIO1 (TX) and GPIO3 (RX)

  // Initialize Modbus RTU
  mb.begin(&Serial);
  mb.slave(SLAVE_ID);

  // Add holding registers
  mb.addHreg(TEMP_REG);
  mb.addHreg(HUM_REG);
}

void loop()
{
  // Process Modbus requests
  mb.task();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (!isnan(h) && !isnan(t))
  {
    // Check humidity change (0.1% threshold)
    if (H != h)
    {
      Serial.print("New Humidity: ");
      MbSerial.print("New Humidity: ");
      Serial.print(h);
      MbSerial.print(h);
      Serial.println("%");
      MbSerial.println("%");
      mb.Hreg(HUM_REG, (int)(h * 10)); // Humidity in 0.1%
      H = h;                           // Update stored value
    }

    // Check temperature change (0.1°C threshold)
    if (T != t)
    {
      Serial.print("New Temperature: ");
      MbSerial.print("New Temperature: ");
      Serial.print(t);
      MbSerial.print(t);
      Serial.println("°C");
      MbSerial.println("°C");
      mb.Hreg(TEMP_REG, (int)(t * 10)); // Temperature in 0.1°C
      T = t;                            // Update stored value
    }
  }
  else
  {
    Serial.println("Failed to read DHT sensor!");
    delay(2000);
  }
  delay(1);
}
