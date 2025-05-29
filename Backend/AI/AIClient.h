#pragma once

#include <drogon/HttpClient.h>
#include <json/json.h>
#include <functional>

using json = Json::Value;

class AIClient
{
public:
    AIClient(const std::string &fullUrl,
             const std::string &apiKey,
             const std::string &modelName,
             trantor::EventLoop *loop = nullptr);

    void chatCompletion(
        const Json::Value &messages,
        const Json::Value &extraParams,
        std::function<void(const std::string &)> callback);

    std::string jsonToString(const Json::Value &value) const;

private:
    std::string model_;
    std::string api_key_;
    std::string path_;
    drogon::HttpClientPtr client_;

    void parseUrl(const std::string &fullUrl, std::string &hostOut, std::string &pathOut);
};
