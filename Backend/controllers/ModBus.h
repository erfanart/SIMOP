#pragma once

#include <drogon/WebSocketController.h>
#include "../plugins/ModbusRTUMaster.h"
// #include <drogon/drogon.h>
#include <memory>
#include <atomic>
using namespace drogon;

class ModBus : public drogon::WebSocketController<ModBus>
{
public:
  ModBus();
  ~ModBus(); // Destructor

  void handleNewMessage(const WebSocketConnectionPtr &,
                        std::string &&,
                        const WebSocketMessageType &) override;
  void handleNewConnection(const HttpRequestPtr &,
                           const WebSocketConnectionPtr &) override;
  void handleConnectionClosed(const WebSocketConnectionPtr &) override;
  WS_PATH_LIST_BEGIN
  WS_PATH_ADD("/ws/modbus");
  // list path definitions here;
  // WS_PATH_ADD("/path", "filter1", "filter2", ...);
  WS_PATH_LIST_END
private:
  void broadcastData(const std::string &message);
  void startDataStream();
  void stopDataStream();

  std::shared_ptr<ModbusRTUMaster> modbus_;
  std::atomic<bool> streaming_active_{false};
  std::thread data_thread_;
  std::unordered_set<drogon::WebSocketConnectionPtr> connections_;
  std::mutex connections_mutex_;
};
