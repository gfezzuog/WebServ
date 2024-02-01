#include "WebServer.h"
#include "Config.hpp"

Configuration::Configuration(){
}

Configuration::~Configuration(){
}

Configuration::Configuration(std::string config){
}

void Configuration::setMap(std::string config){
	std::string::size_type start;
	std::string::size_type end = 0;

	while(end < config.size()){
		start = config.find('{', 0);
		end = config.find('}', 0);
		while(config[end - 1] != '\n')
			end = config.find('}', end + 1);
		if(end != std::string::npos)
			_map.insert(std::pair<std::string, std::string>(config.substr(start + 2, end - 2), config.substr(end + 2)));
		config = config.substr(end + 2);
	}
}

std::string Configuration::GetHostPort()
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

std::string Configuration::GetHost()
{
    std::string hp = GetHostPort();
    return hp.substr(0,  hp.find(':', 0));
}

unsigned int Configuration::GetPort()
{
    std::string hp = GetHostPort();
    //return hp.substr();
}

std::map<std::string, ConfigurationRoute> Configuration::GetConfigsRoute() const
{
}

std::string Configuration::GetServerName()
{
    return _map["server_name"];
}

size_t Configuration::GetLimitSizeBody()
{
    std::string limitSize = _map["max_body_size"].c_str();
    if(limitSize.empty())
        return LLONG_MAX;
    return std::atoi(limitSize.c_str());
}

int Configuration::GetRedirectionCode()
{
    if (!_map["return"].empty())
		return std::atoi(_map["return"].substr(0, 3).c_str());
	return 0;
}

std::string Configuration::GetRedirectionUrl()
{
    //to see
}

std::string Configuration::GetErrorPath(std::string code) const
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