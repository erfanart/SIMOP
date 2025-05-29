#pragma once
#include "AIClient.h"
#include "ToolRegistry.h"
#include <functional>

class ReActAgent
{
public:
    ReActAgent(AIClient &client, ToolRegistry &tools);

    void run(const std::string &userPrompt,
             std::function<void(const std::string &finalResponse)> callback);

private:
    AIClient &ai_;
    ToolRegistry &toolRegistry_;

    void step(const Json::Value &history, std::function<void(const std::string &)> finalCallback);
};
