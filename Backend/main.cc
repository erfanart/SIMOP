#include <drogon/drogon.h>
#include <filesystem>
#include "plugins/AppPaths.h"

int main()
{
    // Set HTTP listener address and port
    // drogon::app().addListener("0.0.0.0", 6080);
    // Load config file
    AppPaths p;
    // std::cout << p.getConfigPath();
    drogon::app()
        .loadConfigFile("./config.yaml");
    
    // drogon::app().loadConfigFile(p.getConfigPath().string() + "/config.yaml");
    // // Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
