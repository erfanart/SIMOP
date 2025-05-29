#include "ReActAgent.h"
#include <regex>
#include <sstream>
#include <fstream>
#include <drogon/drogon.h>

std::string loadPrompt(const std::string &toolsDescription, const std::string &filename = "prompt.txt")
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open prompt template file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string prompt = buffer.str();

    const std::string placeholder = "{{TOOLS}}";
    size_t pos = prompt.find(placeholder);
    if (pos == std::string::npos)
    {
        throw std::runtime_error("Placeholder '{{TOOLS}}' not found in prompt template");
    }

    prompt.replace(pos, placeholder.length(), toolsDescription);
    return prompt;
}

ReActAgent::ReActAgent(AIClient &client, ToolRegistry &tools)
    : ai_(client), toolRegistry_(tools) {}

void ReActAgent::run(const std::string &userPrompt,
                     std::function<void(const std::string &finalResponse)> callback)
{
    const std::string promptFile = drogon::app().getCustomConfig()["ai"]["promptFile"].asString();
    const std::string systemPrompt = loadPrompt(toolRegistry_.listToolDescriptions(), promptFile);

    Json::Value history(Json::arrayValue);

    Json::Value system;
    system["role"] = "system";
    system["content"] = systemPrompt;
    history.append(system);

    Json::Value user;
    user["role"] = "user";
    user["content"] = userPrompt;
    history.append(user);

    step(history, std::move(callback));
}

void ReActAgent::step(const Json::Value &history, std::function<void(const std::string &)> finalCallback)
{
    Json::Value historyCopy = history;
    auto callbackCopy = std::move(finalCallback);

    ai_.chatCompletion(history, {}, [this, historyCopy = std::move(historyCopy), callbackCopy = std::move(callbackCopy)](const std::string &responseText)
                       {
        Json::Value full;
        Json::CharReaderBuilder reader;
        std::string errs;
        std::istringstream s(responseText);
        if (!Json::parseFromStream(reader, s, &full, &errs))
        {
            callbackCopy("❌ JSON parse error: " + errs);
            return;
        }

        const auto &choices = full["choices"];
        if (!choices.isArray() || choices.empty())
        {
            if (full.isMember("error") && full["error"].isObject())
            {
                callbackCopy("❌ Server error: " + full["error"]["message"].asString());
            }
            else
            {
                callbackCopy(responseText);
            }
            return;
        }

        const std::string replyStr = choices[0]["message"]["content"].asString();

        std::smatch match;
        if (std::regex_search(replyStr, match, std::regex(R"(Action:\s*(\w+)\s*(?:\((.*?)\))?)")))
        {
            const std::string command = match[1];
            std::string input = match[2];

            // Sanitize input (remove surrounding quotes if any)
            if (!input.empty() && input.front() == '"' && input.back() == '"')
            {
                input = input.substr(1, input.length() - 2);
            }

            const std::string result = toolRegistry_.runTool(command, input);

            if (result.rfind("❌ Unknown tool", 0) == 0)
            {
                callbackCopy(replyStr);  // let model see the error
                return;
            }

            Json::Value newHistory = historyCopy;

            Json::Value assistant;
            assistant["role"] = "assistant";
            assistant["content"] = replyStr;
            newHistory.append(assistant);

            Json::Value observation;
            observation["role"] = "user";
            observation["content"] = "Observation: " + result;
            newHistory.append(observation);

            this->step(newHistory, callbackCopy);  // next step
        }
        else
        {
            callbackCopy(replyStr);  // final answer
        } });
}
