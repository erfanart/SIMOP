#include "ModbusRTUMaster.h"
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <thread>
#include <chrono>
#include <cerrno>

ModbusRTUMaster::ModbusRTUMaster(const std::string &port, int baudrate, char parity,
                                 int data_bits, int stop_bits, int slave_id,
                                 int temp_reg, int hum_reg, int rpm_reg)
    : m_slave_id(slave_id), m_temp_reg(temp_reg), m_hum_reg(hum_reg), m_rpm_reg(rpm_reg)
{
    m_ctx = modbus_new_rtu(port.c_str(), baudrate, parity, data_bits, stop_bits);
    if (m_ctx == nullptr)
    {
        throw std::runtime_error("Unable to create Modbus context");
    }

    modbus_set_slave(m_ctx, m_slave_id);

    modbus_set_response_timeout(m_ctx, 3, 0);      // 3s timeout
    modbus_set_byte_timeout(m_ctx, 0, 500000);     // 500µs between bytes
}

ModbusRTUMaster::~ModbusRTUMaster()
{
    if (m_ctx != nullptr)
    {
        modbus_close(m_ctx);
        modbus_free(m_ctx);
        m_ctx = nullptr;
    }
}

bool ModbusRTUMaster::connect()
{
    if (m_ctx == nullptr)
        return false;

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
    if (!m_ctx)
        return false;

    modbus_flush(m_ctx);

    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    int result = modbus_read_registers(m_ctx, reg_addr, count, dest);

    if (result != count)
    {
        std::cerr << "Modbus read error: " << modbus_strerror(errno) << std::endl;
        modbus_close(m_ctx);
        if (!modbus_connect(m_ctx))
        {
            std::cerr << "Modbus reconnection failed.\n";
        }
        return false;
    }

    return true;
}

bool ModbusRTUMaster::read_sensor_data(float &temperature, float &humidity)
{
    uint16_t temp_raw, hum_raw;

    if (!read_registers(m_temp_reg, 1, &temp_raw))
        return false;

    if (!read_registers(m_hum_reg, 1, &hum_raw))
        return false;

    temperature = temp_raw / 10.0f;
    humidity = hum_raw / 10.0f;

    return true;
}

bool ModbusRTUMaster::write_single_register(int reg, uint16_t value)
{
    if (!m_ctx)
        return false;

    int rc = modbus_write_register(m_ctx, reg, value);

    if (rc != 1)
    {
        std::cerr << "Write failed: " << modbus_strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool ModbusRTUMaster::read_rpm(int &rpm)
{
    uint16_t value;

    if (!read_registers(m_rpm_reg, 1, &value))
        return false;

    rpm = static_cast<int>(value);
    return true;
}
