#pragma once

#include <string>
#include <json/json.h>

namespace Utils {

    // --- Case Conversion ---
    std::string to_upper(const std::string& str);
    std::string to_lower(const std::string& str);

    // --- Trimming ---
    void ltrim(std::string& s);
    void rtrim(std::string& s);
    void trim(std::string& s);
    std::string trimmed(const std::string& s);

    // --- Prefix/Suffix Checking ---
    bool starts_with(const std::string& str, const std::string& prefix);
    bool ends_with(const std::string& str, const std::string& suffix);

    // --- JSON ---
    std::string jsonToString(const Json::Value& value);

} // namespace utils
