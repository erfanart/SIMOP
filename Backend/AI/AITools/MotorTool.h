#pragma once
#include "../Tool.h"
#include "../../plugins/ModbusRTUMaster.h"
#include <memory>
#include <optional>
#include <string>
#include <algorithm>
#include <cctype>

class MotorTool : public Tool
{
public:
    explicit MotorTool(std::shared_ptr<ModbusRTUMaster> modbus)
        : modbus_(std::move(modbus)) {}

    std::string name() const override { return "motor"; }

    std::string description() const override
    {
        return R"(
This tool controls the motor via Modbus. Usage:
- motor(read) : Read motor RPM and speed.
- motor(run)  : Start the motor.
- motor(stop) : Stop the motor.
- motor(crotate) : Rotated to Clockwise the motor.
- motor(urotate) : Rotated to UnClockwise the motor.
)";
    }

    std::string run(const std::string &input) override
    {
        if (!modbus_)
        {
            return "❌ Modbus connection is not available.";
        }

        std::string action = trim(input);

        if (action == "read")
        {
            uint16_t rpm_raw = 0;
            if (!modbus_->read_registers(RPM_REG, 1, &rpm_raw))
            {
                return "❌ Failed to read RPM register.";
            }
            return "✅ Current RPM: " + std::to_string(rpm_raw);
        }
        else if (action == "run")
        {
            if (!modbus_->write_single_register(MOTOR_REG, 1))
            {
                return "❌ Failed to start motor.";
            }
            return "✅ Motor started.";
        }
        else if (action == "stop")
        {
            if (!modbus_->write_single_register(MOTOR_REG, 0))
            {
                return "❌ Failed to stop motor.";
            }
            return "✅ Motor stopped.";
        }
        else if (action == "crotate")
        {
            if (!modbus_->write_single_register(MOTOR_REG, 2))
            {
                return "❌ Failed to Rotated to Clockwise. motor.";
            }
            return "✅ Motor Rotated to Clockwise.";
        }
        else if (action == "urotate")
        {
            if (!modbus_->write_single_register(MOTOR_REG, 3))
            {
                return "❌ Failed to Rotated to UnClockwise. motor.";
            }
            return "✅ Motor Rotated to UnClockwise.";
        }
        return "❌ Unknown motor action: " + action;
    }

private:
    std::shared_ptr<ModbusRTUMaster> modbus_;

    static constexpr int MOTOR_REG = 102;
    static constexpr int RPM_REG = 103;

    static std::string trim(const std::string &str)
    {
        size_t start = str.find_first_not_of(" \t\n\r");
        size_t end = str.find_last_not_of(" \t\n\r");

        if (start == std::string::npos || end == std::string::npos)
            return "";

        return str.substr(start, end - start + 1);
    }
};
