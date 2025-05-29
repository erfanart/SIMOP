#pragma once
#include "../Tool.h"
#include "../../plugins/ModbusRTUMaster.h"
#include <memory>
#include <optional>
#include <string>

class TempTool : public Tool
{
public:
    explicit TempTool(std::shared_ptr<ModbusRTUMaster> modbus)
        : modbus_(std::move(modbus)) {}

    std::string name() const override { return "read_temp"; }
    std::string description() const override { return "Read temperature from sensor."; }

    std::string run(const std::string &) override
    {
        if (!modbus_)
            return "ماژول Modbus مقداردهی نشده است.";

        float temp = -1.0f;
        float humidityRaw = 0.0f;
        std::optional<float> temperature;

        if (modbus_->read_sensor_data(temp, humidityRaw))
        {
            temperature = temp;
        }

        if (!temperature.has_value())
        {
            return "ماژول در دسترس نیست";
        }

        return std::to_string(*temperature) + "°C";
    }

private:
    std::shared_ptr<ModbusRTUMaster> modbus_;
};
