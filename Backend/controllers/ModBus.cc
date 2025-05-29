#include "ModBus.h"

#define SLAVE_ID 1
#define TEMP_REG 100
#define HUM_REG 101
#define MOTOR_REG 102
#define RPM_REG 103

ModBus::ModBus()
    : modbus_(std::make_shared<ModbusRTUMaster>("/dev/ttyAMA0", 115200, 'N', 8, 1, SLAVE_ID))
{
    if (!modbus_->connect())
    {
        LOG_ERROR << "Failed to connect to Modbus device";
    }
    startDataStream(); // Start automatic streaming on construction
}

ModBus::~ModBus()
{
    stopDataStream();
}

void ModBus::broadcastData(const std::string &message)
{
    std::lock_guard<std::mutex> lock(connections_mutex_);
    for (const auto &conn : connections_)
    {
        conn->send(message);
    }
}

void ModBus::startDataStream()
{
    streaming_active_ = true;
    data_thread_ = std::thread([this]()
                               {
        while (streaming_active_)
        {
            float temp, humidity;
            int rpm;
            if (modbus_->read_sensor_data(temp, humidity) && modbus_->read_rpm(rpm))
            {
                Json::Value data;
                data["temperature"] = temp;
                data["humidity"] = humidity;
                data["rpm"] = rpm;

                drogon::app().getLoop()->queueInLoop([this, data]()
                {
                    broadcastData(data.toStyledString());
                });
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } });
}

void ModBus::stopDataStream()
{
    streaming_active_ = false;
    if (data_thread_.joinable())
    {
        data_thread_.join();
    }
}

void ModBus::handleNewMessage(const WebSocketConnectionPtr &wsConnPtr, std::string &&message, const WebSocketMessageType &type)
{
    Json::Value jsonMsg;
    Json::Reader reader;

    if (reader.parse(message, jsonMsg))
    {
        if (jsonMsg.isMember("command"))
        {
            std::string cmd = jsonMsg["command"].asString();
            if (cmd == "start")
            {
                startDataStream();
                wsConnPtr->send("{\"status\":\"streaming started\"}");
            }
            else if (cmd == "stop")
            {
                stopDataStream();
                wsConnPtr->send("{\"status\":\"streaming stopped\"}");
            }
        }

        if (jsonMsg.isMember("commands") && jsonMsg["commands"].isMember("motor"))
        {
            std::string action = jsonMsg["commands"]["motor"].asString();
            std::transform(action.begin(), action.end(), action.begin(), ::tolower);

            bool success = false;
            if (action == "on")
            {
                success = modbus_->write_single_register(MOTOR_REG, 1);
            }
            else if (action == "off")
            {
                success = modbus_->write_single_register(MOTOR_REG, 0);
            }

            Json::Value res;
            res["motor"] = success ? "✅ success" : "❌ failed";
            wsConnPtr->send(res.toStyledString());
        }
    }
}

void ModBus::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr &wsConnPtr)
{
    LOG_INFO << "New WebSocket connection";
    float temp, humidity;
    int rpm;

    {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        connections_.insert(wsConnPtr);
    }

    if (modbus_->read_sensor_data(temp, humidity) && modbus_->read_rpm(rpm))
    {
        Json::Value data;
        data["temperature"] = temp;
        data["humidity"] = humidity;
        data["rpm"] = rpm;
        wsConnPtr->send(data.toStyledString());
    }
}

void ModBus::handleConnectionClosed(const WebSocketConnectionPtr &wsConnPtr)
{
    std::lock_guard<std::mutex> lock(connections_mutex_);
    connections_.erase(wsConnPtr);
    LOG_INFO << "WebSocket connection closed";
}
