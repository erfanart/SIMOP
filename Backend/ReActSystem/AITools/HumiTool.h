#pragma once
#include "../Tool.h"
#include "../../plugins/ModbusRTUMaster.h"
#include <memory>
#include <optional>
#include <string>

class HumiTool : public Tool
{
public:
    explicit HumiTool(std::shared_ptr<ModbusRTUMaster> modbus)
        : modbus_(std::move(modbus)) {}

    std::string name() const override { return "read_humi"; }
    std::string description() const override { return "Read humidity from sensor."; }

    std::string run(const std::string &) override
    {
        if (!modbus_)
            return "ماژول Modbus مقداردهی نشده است.";

        float temp = 0.0f;
        float humidityRaw = -1.0f;
        std::optional<float> humidity;

        if (modbus_->read_sensor_data(temp, humidityRaw))
        {
            humidity = humidityRaw;
        }

        if (!humidity.has_value())
        {
            return "ماژول در دسترس نیست";
        }

        return std::to_string(*humidity) + "%";
    }

private:
    std::shared_ptr<ModbusRTUMaster> modbus_;
};
