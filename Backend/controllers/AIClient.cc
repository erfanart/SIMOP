#include "AIClient.h"
#include <json/reader.h>
#include <json/writer.h>
#include <sstream>
#include <regex>

void AIClient::parseUrl(const std::string &fullUrl, std::string &hostOut, std::string &pathOut)
{
    // Extract scheme, host, and path using regex
    std::regex urlRegex(R"(^(http[s]?)://([^/]+)(/.*)?$)");
    std::smatch match;

    if (std::regex_match(fullUrl, match, urlRegex))
    {
        // match[1] = http or https
        // match[2] = host:port
        // match[3] = path
        hostOut = match[1].str() + "://" + match[2].str();
        pathOut = match[3].matched ? match[3].str() : "/";
    }
    else
    {
        // fallback
        hostOut = fullUrl;
        pathOut = "/";
    }
}

AIClient::AIClient(const std::string &fullUrl,
                   const std::string &apiKey,
                   const std::string &modelName,
                   trantor::EventLoop *loop)
    : model_(modelName), api_key_(apiKey)
{
    std::string host, path;
    parseUrl(fullUrl, host, path);
    path_ = path;
    client_ = drogon::HttpClient::newHttpClient(host, loop);
}

std::string AIClient::jsonToString(const Json::Value &value) const
{
    Json::StreamWriterBuilder writer;
    writer["indentation"] = "";
    return Json::writeString(writer, value);
}

void AIClient::chatCompletion(
    const Json::Value &messages,
    const Json::Value &extraParams,
    std::function<void(const std::string &)> callback)
{
    if (!messages.isArray() || messages.empty())
    {
        LOG_ERROR << "❌ Invalid messages array passed to chatCompletion()";
        callback("❌ Internal error: messages must be a non-empty array.");
        return;
    }

    Json::Value payload;
    payload["model"] = model_;
    payload["messages"] = messages;

    for (const auto &key : extraParams.getMemberNames())
        payload[key] = extraParams[key];

    auto req = drogon::HttpRequest::newHttpJsonRequest(payload);
    req->setMethod(drogon::Post);
    req->setPath(path_);  // ✅ Set the correct endpoint
    req->addHeader("Authorization", "Bearer " + api_key_);
    req->addHeader("Content-Type", "application/json");
    std::cout << "🔄 Sending request to: " << path_ << std::endl;
    std::cout << "Request Body is: " << req->getBody() << std::endl;
    std::cout << "🧾 Request Headers:" << std::endl;
    for (const auto &header : req->getHeaders())
    {
        std::cout << header.first << ": " << header.second << std::endl;
    }
    client_->sendRequest(req, [callback](drogon::ReqResult result, const drogon::HttpResponsePtr &resp)
                         {                  
        if (result == drogon::ReqResult::Ok && resp)
        {
            LOG_DEBUG << "🟢 Response status: " << resp->getStatusCode();
            LOG_DEBUG << "🟢 Response body:\n" << resp->getBody();
            callback(std::string(resp->getBody()));
        }
        else
        {
            LOG_ERROR << "❌ HTTP request failed.";
            LOG_ERROR << "Result: " << static_cast<int>(result);
            if (resp)
                LOG_ERROR << "Response body: " << resp->getBody();
            callback("❌ خطا در ارتباط با سرور مدل");
        } });
}
