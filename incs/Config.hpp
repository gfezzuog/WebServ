#pragma once

# include "WebServer.h"
# include "Routes.hpp"

class ConfigurationRoute;

class Configuration
{
    private:
    std::map<std::string, std::string> _map;
    std::vector<std::string> _methods;
    std::map<std::string, ConfigurationRoute> _configsRoute; 
    void setMap(std::string config);
    void setMethods();
    void setConfigsRoute();

    public:
    Configuration();
    ~Configuration();
    Configuration(std::string config);
    std::string GetHostPort();
    std::string GetHost();
    unsigned int GetPort();
    std::map<std::string, ConfigurationRoute> GetConfigsRoute();
    std::string GetServerName();
    size_t GetLimitSizeBody();
    int GetRedirectionCode();
    std::string GetRedirectionUrl();
    std::string GetErrorPath(std::string code) const;
    bool isMethod(std::string method);
    bool isEmpty();
};
