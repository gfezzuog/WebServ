#pragma once

# include "WebServer.h"

class ConfigurationRoute;

class Configuration
{
    private:
    std::map<std::string, std::string> _map;
    std::vector<std::string> _methods;
    std::map<std::string, ConfigurationRoute> _configsRoute;

    public:
    Configuration();
    ~Configuration();
    Configuration(std::string config);
    std::string GetHostPort();
    std::string GetHost();
    unsigned int GetPort();
    std::map<std::string, ConfigurationRoute> GetConfigsRoute() const;
    std::string GetServerName();
    size_t GetLimitSizeBody();
    int GetRedirectionCode();
    std::string GetRedirectionUrl();
    std::string GetErrorPath(std::string code) const;
};