#include "WebServer.h"

Configuration::Configuration()
{

}

Configuration::~Configuration()
{
    //sets
}

Configuration::Configuration(std::string config)
{

}

std::string GetHostPort()
{
	try
	{
		return _map.at("host/port");
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: Missing host/port attribute in the configuration file" << RESET << std::endl;
		exit(1);
	}
}

std::string GetHost()
{
    std::string hp = GetHostPort();
    return hp.substr(0,  hp.find(':', 0));
}

unsigned int GetPort()
{
    std::string hp = GetHostPort();
    //return hp.substr();
}

std::map<std::string, ConfigurationRoute> GetConfigsRoute() const
{
}

std::string GetServerName()
{
    return _map["server_name"];
}

size_t GetLimitSizeBody()
{
    std::sring limitSize = _map["max_body_size"].c_str();
    if(limitSize.empty())
        return LLONG_MAX;
    return std::atoi(limitSize.c_str());
}

int GetRedirectionCode()
{
    if (!_map["return"].empty())
		return std::atoi(_map["return"].substr(0, 3).c_str());
	return 0;
}

std::string GetRedirectionUrl()
{
    //to see
}

std::string GetErrorPath()
{
    try
	{
		return _map.at(code);
	}
	catch(const std::exception& e)
	{
		return "errors/error_500.html";
	}
}