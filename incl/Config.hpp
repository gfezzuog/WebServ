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
    void printclass(){
        std::cout << "Configuration Details:" << std::endl;
        std::cout << "Host: " << GetHost() << std::endl;
        std::cout << "Port: " << GetPort() << std::endl;
        std::cout << "Server Name: " << GetServerName() << std::endl;
        std::cout << "Limit Size Body: " << GetLimitSizeBody() << std::endl;
        std::cout << "Redirection Code: " << GetRedirectionCode() << std::endl;
        std::cout << "Redirection URL: " << GetRedirectionUrl() << std::endl;
        std::cout << "Error Path: " << GetErrorPath("404") << std::endl;
        std::cout << "End of Configuration" << std::endl;
        for(std::map<std::string, ConfigurationRoute>::iterator it = _configsRoute.begin(); it != _configsRoute.end(); it++)
        {
            std::cout<<it->first<<std::endl;
            it->second.printConfigurationroute();
        }
    };
};
