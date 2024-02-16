#include "../incl/WebServer.h"

Configuration::Configuration(){
}

Configuration::~Configuration(){
}

Configuration::Configuration(std::string config){
	setMap(config);
	setConfigsRoute();
	setMethods();
}

void Configuration::setMap(std::string config){
	std::string::size_type start;
	std::string::size_type end = 0;

	while(end < config.size()){
		start = config.find('=', 0);
		if(config.substr(1, start - 1) == "routes"){
			end = config.find('}', 0);
			while(config.find('}', end + 1) != std::string::npos)
				end = config.find('}', end + 1);
			if(end != std::string::npos)
				_map.insert(std::pair<std::string, std::string>(config.substr(1, start - 1), config.substr(start + 1, end - start)));
			config = config.substr(end, 0);
			if(end != std::string::npos)
				end = config.find('\n', end);
			else
				end = config.find('\n', 0);
		}
		else {
			end = config.find('\n', 0);
			if(end != std::string::npos)
				_map.insert(std::pair<std::string, std::string>(config.substr(1, start - 1), config.substr(start + 1, end - start - 1)));
			config = config.substr(end + 1);
		}
	}
	start = config.find('=', 0);
	end = config.find('\0', 0);
	if(start != std::string::npos)
		_map.insert(std::pair<std::string, std::string>(config.substr(1, start - 1), config.substr(start + 1, end - 1)));
	// for (std::map<std::string, std::string>::iterator i = _map.begin() ; i != _map.end(); i++)
	// 	std::cout << GREEN << "first: " << RESET << std::left << std::setw(20) << std::setfill(' ') << (*i).first << GREEN << " second: " << RESET << (*i).second << std::endl <<
	// 	"----------------------------------------------------------------------------------"<< std::endl;
}

void Configuration::setMethods(){
	std::string::size_type start;
	std::string::size_type end = 0;

	while(end < _map["methods"].size()){
		start = 0;
		end = _map["methods"].find(' ', 0);
		if(end != std::string::npos){
			_methods.push_back(_map["methods"].substr(start, end));
			_map["methods"] = _map["methods"].substr(end + 1);
		}
	}
	_methods.push_back(_map["methods"].substr(0, _map["methods"].size()));
}

void Configuration::setConfigsRoute()
{
	std::cout << "setConfigsRoute" << std::endl;
	if(_map.find("routes") == _map.end())
		return;
	std::string::size_type start;
	std::string::size_type end = 0;
	std::string::size_type i;
	std::cout << "setConfigsRoute FIRST iteration BEFORE while" << std::endl;
	std::string config = _map["routes"];
	std::cout << "setConfigsRoute ends with "<< config << std::endl;
	while(end < config.size()){
		std::cout << "setConfigsRoute first while" << std::endl;
		i = 0;
		while(isspace(config[i]))
			i++;
		start = config.find('{', 0);
		end = config.find('}', 0);
		std::cout << "setConfigsRoute before if inside while" << std::endl;
		if(end != std::string::npos){
			_configsRoute.insert(std::pair<std::string, ConfigurationRoute>(config.substr(i, start - i - 1), ConfigurationRoute(config.substr(start, end - start), config.substr(i, start - i - 1))));
			config = config.substr(end + 1);
		}
		//printconfigsroute();
		for(std::map<std::string, ConfigurationRoute>::iterator it = _configsRoute.begin(); it != _configsRoute.end(); it++)
		{
			std::cout << "setConfigsRoute: " << it->first << std::endl;
			it->second.printConfigurationroute();
		}
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
    return std::atoi(hp.substr(hp.find(':', 0) + 1, hp.size()).c_str());
}

std::map<std::string, ConfigurationRoute> Configuration::GetConfigsRoute()
{
	// std::cout<<"-----------------GetConfigsRoute-----------------"<<std::endl;
	// for(std::map<std::string, ConfigurationRoute>::iterator it = _configsRoute.begin(); it != _configsRoute.end(); it++)
	// {
	// 	std::cout << "GetConfigsRoute: " << it->first << std::endl;
	// 	it->second.printConfigurationroute();
	// }
	return _configsRoute;
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
    std::string redir = _map["return"];
	if(redir.empty()){
		int start = redir.find(' ', 0) + 1;
		return _map["return"].substr(start, redir.size() - start);
	}
	return std::string();
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

bool Configuration::isMethod(std::string method){
	return _methods.end() != std::find(_methods.begin(), _methods.end(), method);
}

bool Configuration::isEmpty(){
	return _map.size() == 0;
}