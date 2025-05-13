#include "ModbusRTUMaster.h"
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <modbus/modbus.h>
#include <thread> // Add this for std::this_thread
#include <chrono> // Add this for std::chrono

ModbusRTUMaster::ModbusRTUMaster(const std::string &port, int baudrate, char parity,
                                 int data_bits, int stop_bits, int slave_id,
                                 int temp_reg, int hum_reg)
    : m_slave_id(slave_id), m_temp_reg(temp_reg), m_hum_reg(hum_reg)
{

    // Create Modbus context
    m_ctx = modbus_new_rtu(port.c_str(), baudrate, parity, data_bits, stop_bits);
    if (m_ctx == nullptr)
    {
        throw std::runtime_error("Unable to create Modbus context");
    }

    // Set slave ID
    modbus_set_slave(m_ctx, m_slave_id);

    // Set response timeout
    modbus_set_response_timeout(m_ctx, 3, 0);  // 3 second response timeout
    modbus_set_byte_timeout(m_ctx, 0, 500000); // 500µs between bytes
}

ModbusRTUMaster::~ModbusRTUMaster()
{
    if (m_ctx != nullptr)
    {
        modbus_close(m_ctx);
        modbus_free(m_ctx);
    }
}

bool ModbusRTUMaster::connect()
{
    return modbus_connect(m_ctx) == 0;
}

void ModbusRTUMaster::disconnect()
{
    if (m_ctx != nullptr)
    {
        modbus_close(m_ctx);
    }
}

bool ModbusRTUMaster::read_registers(int reg_addr, int count, uint16_t *dest)
{
    modbus_flush(m_ctx);

    // Add delay between requests
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    // Attempt read
    int result = modbus_read_registers(m_ctx, reg_addr, count, dest);

    // Handle failure
    if (result != count)
    {
        // std::cerr << "Modbus error: " << modbus_strerror(errno) << std::endl;
        modbus_close(m_ctx);
        if (!modbus_connect(m_ctx))
        {
            // std::cerr << "Reconnected to Modbus device" << std::endl;
        }
        return false;
    }

    return true;
}

bool ModbusRTUMaster::read_sensor_data(float &temperature, float &humidity)
{
    uint16_t temp_raw, hum_raw;

    // std::cout << "Attempting to read temperature..." << std::endl;
    if (!read_registers(m_temp_reg, 1, &temp_raw))
    {
        // std::cerr << "Temperature read failed with error: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    // std::cout << "Attempting to read humidity..." << std::endl;
    if (!read_registers(m_hum_reg, 1, &hum_raw))
    {
        // std::cerr << "Humidity read failed with error: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    temperature = temp_raw / 10.0f;
    humidity = hum_raw / 10.0f;

    return true;
}