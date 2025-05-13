#pragma once
#include <drogon/WebSocketController.h>
#include "AIClient.h"
#include "../ReActSystem/ReActAgent.h"
#include "../ReActSystem/ToolRegistry.h"
#include <regex>
#include "../plugins/Utils.h"

using namespace drogon;
class AiChat : public drogon::WebSocketController<AiChat>
{
public:
  AiChat();

  void handleNewMessage(const drogon::WebSocketConnectionPtr &, std::string &&, const drogon::WebSocketMessageType &) override;
  void handleNewConnection(const drogon::HttpRequestPtr &, const drogon::WebSocketConnectionPtr &) override;
  void handleConnectionClosed(const drogon::WebSocketConnectionPtr &) override;

  WS_PATH_LIST_BEGIN
  WS_PATH_ADD("/ws/chat", Get);
  WS_PATH_LIST_END

private:
  AIClient aiAgent_;
  ToolRegistry tools_;
};
