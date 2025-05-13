#include "Utils.h"
#include <algorithm>
#include <cctype>

namespace Utils {

    std::string to_upper(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    std::string to_lower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            [](unsigned char c) { return !std::isspace(c); }));
    }

    void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
            [](unsigned char c) { return !std::isspace(c); }).base(), s.end());
    }

    void trim(std::string& s) {
        ltrim(s);
        rtrim(s);
    }

    std::string trimmed(const std::string& s) {
        std::string result = s;
        trim(result);
        return result;
    }

    bool starts_with(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() &&
               std::equal(prefix.begin(), prefix.end(), str.begin());
    }

    bool ends_with(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() &&
               std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
    }

    std::string jsonToString(const Json::Value& value) {
        Json::StreamWriterBuilder writer;
        writer["indentation"] = "";  // Compact JSON
        return Json::writeString(writer, value);
    }

} // namespace utils
