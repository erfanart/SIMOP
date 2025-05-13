
#include "AiChat.h"
#include "../ReActSystem/AITools/TempTool.h"
#include "../ReActSystem/AITools/HumiTool.h"
#include <algorithm>
#include <drogon/drogon.h>


using namespace drogon;
std::string extractAnswerWithRegex(const std::string &text)
{
    std::smatch match;
    std::regex answerRegex(R"(Answer:\s*(.*))");
    if (std::regex_search(text, match, answerRegex))
    {
        return match[1].str();
    }
    return  text;
}

AiChat::AiChat()
    : aiAgent_(
        drogon::app().getCustomConfig()["ai"]["url"].asString(),
        drogon::app().getCustomConfig()["ai"]["token"].asString(),
        drogon::app().getCustomConfig()["ai"]["modelName"].asString())
{
    auto modbus = std::make_shared<ModbusRTUMaster>("/dev/ttyAMA0", 115200, 'N', 8, 1, 1);
    tools_.registerTool(std::make_shared<TempTool>(modbus));
    tools_.registerTool(std::make_shared<HumiTool>(modbus));

    LOG_INFO << "Token: " << drogon::app().getCustomConfig()["ai"]["token"].asString();
}

void AiChat::handleNewConnection(const HttpRequestPtr &req, const WebSocketConnectionPtr &wsConnPtr)
{
    LOG_INFO << "New WebSocket connection from: " << req->getPeerAddr().toIpPort();
}

void AiChat::handleNewMessage(const WebSocketConnectionPtr &wsConnPtr, std::string &&message, const WebSocketMessageType &type)
{
    switch (type)
    {
    case WebSocketMessageType::Text:
        break;

    case WebSocketMessageType::Binary:
        wsConnPtr->send("❌ فقط پیام‌های متنی پشتیبانی می‌شوند");
        return;

    case WebSocketMessageType::Ping:
    case WebSocketMessageType::Pong:
        return;

    default:
        wsConnPtr->send("❌ نوع پیام پشتیبانی نمی‌شود");
        return;
    }

    // بررسی فقط برای خالی نبودن
    std::string trimmed = message;
    LOG_INFO << "📨 پیام کاربر دریافت شد: [" << message << "]";

    trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), ::isspace), trimmed.end());
    if (trimmed.empty())
        return;

    // 🔥 استفاده از message اصلی، نه trimmed
    ReActAgent agent(aiAgent_, tools_);

    agent.run(message, [wsConnPtr](const std::string &finalAnswer)
              { wsConnPtr->send(extractAnswerWithRegex(finalAnswer)); });
}

void AiChat::handleConnectionClosed(const WebSocketConnectionPtr &wsConnPtr)
{
    LOG_INFO << "WebSocket connection closed.";
}
