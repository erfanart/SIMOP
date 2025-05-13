#include <ESP8266WiFi.h>
#include <ModbusIP_ESP8266.h> // Modbus TCP library
#include <DHT.h>

// WiFi credentials
#ifndef STASSID
#define STASSID "EarlyBird"
#define STAPSK "1379.Erfan"
#endif
// DHT11 setup

#define DPIN 14     // Pin to connect DHT sensor (GPIO number) D2
#define DTYPE DHT11 // Define DHT 11 or DHT22 sensor type
DHT dht(DPIN, DTYPE);

// Modbus object
ModbusIP mb;

const char *ssid = STASSID;
const char *password = STAPSK;
IPAddress local_IP(10, 140, 0, 9);
IPAddress gateway(10, 140, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(10, 140, 0, 1);

void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_AP_STA);
  //   if (!WiFi.config(local_IP, gateway, subnet,dns)) {
  //    Serial.println("STA Failed to configure");
  //  }
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    //    digitalWrite(LED_BUILTIN,HIGH);
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// Holding register to store sensor values
const int TEMP_REG = 100; // Register to hold temperature value
const int HUM_REG = 101;  // Register to hold humidity value

void setup()
{
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Connect to WiFi
  setup_wifi();
  // Start Modbus TCP server
  mb.server();
}

void loop()
{
  // Poll for Modbus communication
  mb.task();
  // Serial.println("Wait for get data from dht11:");

  // Read temperature and humidity from DHT11
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // If there's a valid reading
  if (!isnan(h) && !isnan(t))
  {
    // If the registers don't exist yet, add them
    if (!mb.Hreg(TEMP_REG))
    {
      mb.addHreg(TEMP_REG);
    }
    if (!mb.Hreg(HUM_REG))
    {
      mb.addHreg(HUM_REG);
    }

    // Update Modbus registers with new values
    mb.Hreg(TEMP_REG, (int)(t * 10)); // Temperature in 0.1°C units
    mb.Hreg(HUM_REG, (int)(h * 10));  // Humidity in 0.1% units
    delay(1000);
    Serial.print("Temp: ");
    Serial.print(t);
    Serial.print(" °C, Humidity: ");
    Serial.print(h);
    Serial.println(" %");
  }

  // Delay for 2 seconds between readings
  delay(1000);
}
