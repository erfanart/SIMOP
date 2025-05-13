#ifndef APPPATHS_H
#define APPPATHS_H

#include <filesystem>
#include <vector>
#include <optional>
#include <string>
#include "./Utils.h"
namespace fs = std::filesystem;

class AppPaths
{
public:
    AppPaths(const std::string &appName = "SIMOP");

    // Explicitly disable copying
    AppPaths(const AppPaths &) = delete;
    AppPaths &operator=(const AppPaths &) = delete;

    // Allow moving
    AppPaths(AppPaths &&) = default;
    AppPaths &operator=(AppPaths &&) = default;

    // Accessors for various paths
    fs::path getConfigPath() const;
    fs::path getDataPath() const;
    fs::path getLogPath() const;
    fs::path getCachePath() const;

    // Get the resolved base directory
    const fs::path &getBaseDir() const;

private:
    std::string appName;
    // Creates a path and ensures it exists
    fs::path makePath(const fs::path &relative) const;

    void initializeBaseDir();
    std::vector<fs::path> getPlatformCandidates() const;
    std::optional<fs::path> getPathFromEnv(const char *varName) const;
    bool validatePath(const fs::path &path) const;

    fs::path baseDir;
};

#endif // APPPATHS_H