#include "AppPaths.h"
#include <fstream>
#include <stdexcept>
#include <cstdlib>
#include <algorithm>

AppPaths::AppPaths(const std::string &appName) : appName(appName.empty() ? "SIMOP" : appName)
{
    initializeBaseDir();
}

fs::path AppPaths::getConfigPath() const { return makePath("config"); }
fs::path AppPaths::getDataPath() const { return makePath("data"); }
fs::path AppPaths::getLogPath() const { return makePath("logs"); }
fs::path AppPaths::getCachePath() const { return makePath("cache"); }

const fs::path &AppPaths::getBaseDir() const { return baseDir; }

fs::path AppPaths::makePath(const fs::path &relative) const
{
    auto fullPath = baseDir / relative;
    fs::create_directories(fullPath);
    return fullPath;
}

void AppPaths::initializeBaseDir()
{
    // Try environment variable first
    if (auto envPath = getPathFromEnv((Utils::to_upper(appName) + "_BASE_DIR").c_str()))
    {
        if (validatePath(*envPath))
        {
            baseDir = *envPath;
            return;
        }
    }

    // Platform-specific candidates
    auto candidates = getPlatformCandidates();

    // Add current directory as final fallback
    candidates.push_back(fs::current_path());

    // Find first valid path
    for (const auto &candidate : candidates)
    {
        if (validatePath(candidate))
        {
            baseDir = candidate;
            return;
        }
    }

    throw std::runtime_error("Could not determine valid base directory");
}

std::vector<fs::path> AppPaths::getPlatformCandidates() const
{
    std::vector<fs::path> candidates;

#ifdef _WIN32
    if (auto programData = getPathFromEnv("PROGRAMDATA"))
    {
        candidates.push_back(*programData / appName);
    }
    if (auto localAppData = getPathFromEnv("LOCALAPPDATA"))
    {
        candidates.push_back(*localAppData / appName);
    }
#else
    // UNIX-like systems (Linux, macOS, BSD, etc.)
    if (auto xdgConfig = getPathFromEnv("XDG_CONFIG_HOME"))
    {
        candidates.push_back(*xdgConfig / appName);
    }
    if (auto xdgData = getPathFromEnv("XDG_DATA_HOME"))
    {
        candidates.push_back(*xdgData / appName);
    }
    if (auto home = getPathFromEnv("HOME"))
    {
        candidates.push_back(*home / (".config/" + appName).c_str());
        candidates.push_back(*home / (".local/share/" + appName).c_str());
        candidates.push_back(*home / ("." + appName).c_str());
    }
#endif

    return candidates;
}

std::optional<fs::path> AppPaths::getPathFromEnv(const char *varName) const
{
    const char *envValue = std::getenv(varName);
    if (!envValue || !*envValue)
    {
        return std::nullopt;
    }

    try
    {
        fs::path path(envValue);
        return path.empty() ? std::nullopt : std::make_optional(path.lexically_normal());
    }
    catch (const fs::filesystem_error &)
    {
        return std::nullopt;
    }
}

bool AppPaths::validatePath(const fs::path &path) const
{
    try
    {
        // Create directory if it doesn't exist
        fs::create_directories(path);

        // Test write access
        auto testFile = path / ".tmp_test_file";
        {
            std::ofstream file(testFile);
            if (!file)
                return false;
            file << "test";
        }
        fs::remove(testFile);

        return true;
    }
    catch (...)
    {
        return false;
    }
}