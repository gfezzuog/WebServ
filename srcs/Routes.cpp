#include "../incl/WebServer.h"

ConfigurationRoute::ConfigurationRoute(){
}

ConfigurationRoute::ConfigurationRoute(std::string config, std::string path): _path(path){
	setMap(config);
}

ConfigurationRoute::~ConfigurationRoute(){
}

void ConfigurationRoute::setMap(std::string config){
	std::string::size_type start;
	std::string::size_type end = 0;
	std::string::size_type i;

	config = config.substr(1, config.size() - 2);
	while(end < config.size()){
		i = 0;
		while(isspace(config[i]))
			i++;
		start = config.find('=', 0);
		end = config.find('\n', 0);
		if(end != std::string::npos){
			_map.insert(std::pair<std::string, std::string>(config.substr(i, start - i), config.substr(start + 1, end - start - 1)));
			config = config.substr(end + 1);
		}
	}
}

std::string ConfigurationRoute::GetPath(){
	return _path;
}

std::string ConfigurationRoute::GetRoot(){
	return _map["root"];
}

std::vector<std::string> ConfigurationRoute::GetCGIPath(){
	std::vector<std::string> cgi;
	int delimiter = _map["cgi_pass"].find(' ');
	cgi.push_back(_map["cgi_pass"].substr(0, delimiter));
	cgi.push_back(_map["cgi_pass"].substr(delimiter + 1, _map["cgi_pass"].size() - delimiter - 1));
	return cgi;
}

std::string ConfigurationRoute::GetUploadPath(){
	std::string uploadPath = _map["upload_directory"];
	if (uploadPath.empty())
		return std::string();
	return uploadPath;
}

bool ConfigurationRoute::GetAutoIndex(){
	return _map["autoindex"] == "on";
}

std::string ConfigurationRoute::GetMethods(){
	return _map["methods"];
}

std::string ConfigurationRoute::GetIndex(){
	return _map["index"];
}