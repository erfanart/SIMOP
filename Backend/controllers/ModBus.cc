#include "ModBus.h"
ModBus::ModBus()
    : modbus_(std::make_shared<ModbusRTUMaster>("/dev/ttyAMA0", 115200, 'N', 8, 1, 1))
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
        while (streaming_active_) {
            float temp, humidity;
            if (modbus_->read_sensor_data(temp, humidity)) {
                Json::Value data;
                data["temperature"] = temp;
                data["humidity"] = humidity;
                
                drogon::app().getLoop()->queueInLoop([this, data]() {
                    broadcastData(data.toStyledString());
                });
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
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
            if (jsonMsg["command"] == "start")
            {
                startDataStream();
                wsConnPtr->send("{\"status\":\"streaming started\"}");
            }
            else if (jsonMsg["command"] == "stop")
            {
                stopDataStream();
                wsConnPtr->send("{\"status\":\"streaming stopped\"}");
            }
        }
    }
}

void ModBus::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr &wsConnPtr)
{
    // write your application logic here
    LOG_INFO << "New WebSocket connection";
    float temp, humidity;
    {
        std::lock_guard<std::mutex> lock(connections_mutex_);
        connections_.insert(wsConnPtr);
    }

    if (modbus_->read_sensor_data(temp, humidity))
    {
        Json::Value data;
        data["temperature"] = temp;
        data["humidity"] = humidity;
        wsConnPtr->send(data.toStyledString());
    }
}

void ModBus::handleConnectionClosed(const WebSocketConnectionPtr &wsConnPtr)
{
    std::lock_guard<std::mutex> lock(connections_mutex_);
    connections_.erase(wsConnPtr);
    // write your application logic here
    LOG_INFO << "WebSocket connection closed";
}
