#include "ReActAgent.h"
#include <regex>
#include <sstream>

ReActAgent::ReActAgent(AIClient &client, ToolRegistry &tools)
    : ai_(client), toolRegistry_(tools) {}

void ReActAgent::run(const std::string &userPrompt,
                     std::function<void(const std::string &finalResponse)> callback)
{
    //LOG_INFO << userPrompt;
    //LOG_INFO << "Registered Tools: " << toolRegistry_.listToolDescriptions();
    const std::string systemPrompt = R"(
        You must follow this reasoning loop:
        Thought → Action → PAUSE → Observation → [repeat if needed] → Answer
        
        Rules:
        - Use only English tool names (e.g. read_temp, calculate)
        - Always respond in the same language as the user's question
        - After each Action, reply only with: PAUSE
        - When you reach a conclusion, respond with: Answer: <your final reply>
        
        Definitions:
        - Thought: What you're thinking
        - Action: Call a tool using Action: <tool>(<input>)
        - Observation: Result from the tool
        - Answer: Final natural-language response to the user
        
        Available tools:
        )" + toolRegistry_.listToolDescriptions() + R"(
        
        Example:
        Q: دما چقدر است؟
        Thought: باید دما را از سنسور بخوانم.
        Action: read_temp()
        PAUSE
        Observation: 29°C
        Answer: دمای فعلی ۲۹ درجه سانتی‌گراد است.
        
        Start the session now. Stick to the format. Only use Answer: for final replies.
        )";
        
    // const std::string systemPrompt = "You are a reasoning assistant";

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
    Json::Value historyCopy = history; // clone since we're mutating
    //LOG_INFO << "req is: " << ai_.jsonToString(history);
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
                const std::string errMsg = full["error"]["message"].asString();
                callbackCopy("❌ Server error: " + errMsg);
            }
            else
            {
                callbackCopy(responseText);  // fallback
            }
            return;
        }

        const std::string replyStr = choices[0]["message"]["content"].asString();

        std::smatch match;
        if (std::regex_search(replyStr, match, std::regex(R"(Action:\s*(\w+))")))
        {
            const std::string command = match[1];
            const std::string result = toolRegistry_.runTool(command, "");

            if (result.rfind("❌ Unknown tool", 0) == 0)
            {
                callbackCopy(replyStr);  // invalid tool → respond normally
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

            this->step(newHistory, callbackCopy);  // recursive step
        }
        else
        {
            callbackCopy(replyStr);  // normal response
        } });
}
