#include "ModbusRTUMaster.h"
#include <iostream>

int main()
{
    try
    {
        std::cout << "Starting Modbus RTU Master..." << std::endl;

        // Create Modbus master instance
        ModbusRTUMaster master("/dev/ttyAMA2", 115200, 'N', 8, 1, 1);

        if (!master.connect())
        {
            std::cerr << "Modbus connection failed" << std::endl;
            return 1;
        }

        std::cout << "Connected to Modbus device. Reading data every 2 seconds..." << std::endl;

        // Main loop
        while (true)
        {
            float temperature, humidity;

            if (master.read_sensor_data(temperature, humidity))
            {
                std::cout << "Temperature: " << temperature << "°C, "
                          << "Humidity: " << humidity << "%" << std::endl;
            }
            else
            {
                std::cerr << "Error reading sensor data" << std::endl;
            }

            sleep(2); // Poll every 2 seconds
        }

        master.disconnect();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}