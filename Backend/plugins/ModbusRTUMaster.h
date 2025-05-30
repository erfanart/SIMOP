#ifndef MODBUSRTUMASTER_H
#define MODBUSRTUMASTER_H

#include <string>
#include <modbus/modbus.h>

class ModbusRTUMaster
{
private:
    modbus_t *m_ctx;
    int m_slave_id;
    int m_temp_reg;
    int m_hum_reg;
    int m_rpm_reg;

public:
    // Constructor
    ModbusRTUMaster(const std::string &port, int baudrate, char parity,
                    int data_bits, int stop_bits, int slave_id,
                    int temp_reg = 100, int hum_reg = 101, int rpm_reg = 103);

    // Destructor
    ~ModbusRTUMaster();

    // Connect to Modbus device
    bool connect();

    // Disconnect from Modbus device
    void disconnect();

    // Read holding registers
    bool read_registers(int reg_addr, int count, uint16_t *dest);

    // High-level read methods
    bool read_sensor_data(float &temperature, float &humidity);
    bool read_rpm(int &rpm);

    // Write methods
    bool write_single_register(int reg, uint16_t value);
};

#endif // MODBUSRTUMASTER_H
