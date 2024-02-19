#include "../incs/WebServer.h"

ConfigFile::ConfigFile(const std::string& inputfile) : _file(inputfile.c_str()) 
{
	try{
		std::stringstream buffer;
		buffer << _file.rdbuf();
		if(buffer.fail()){
			std::cerr << "Error: Invalid configuration file: " << inputfile << std::endl;
			exit(1);
		}
		_content = buffer.str();
		_configString = splitString(_content);
		std::vector<std::string>::iterator it = _configString.begin();
		for( ; it != _configString.end(); it++)
			_configs.push_back(Configuration(*it));
		setMapConfigs();
	}
	catch(const std::exception& e){
		std::cerr << "Error: Parsing configuration file gone wrong" << std::endl;
		exit(1);
	}
}

ConfigFile::~ConfigFile(){
}

void ConfigFile::setMapConfigs(){
	std::vector<Configuration>::iterator it1 = _configs.begin();
	for( ; it1 != _configs.end(); it1++)
		_mapConfigs[(*it1).GetHostPort()].push_back(*it1);
}

std::vector<std::string> ConfigFile::splitString(std::string content){
	std::vector<std::string> result;
    std::string::size_type start;
	std::string::size_type end = 0;

	while (content.find('}', 0) != std::string::npos)
	{
		start = content.find('{', 0);
		end = content.find('}', 0);
		while (content[end - 1] != '\n')
			end = content.find('}', end + 1);
		if(end != std::string::npos)
			result.push_back(content.substr(start + 2, end - 2));
		content = content.substr(end + 2);
	}
    // for (const auto& str : result) {
    //     std::stringstream ss(str);
    //     char delimiter = '{';

    //     std::string component;
    //     while (std::Getline(ss, component, delimiter)) {
    //         if (!component.empty()) {
    //             dividedComponents.push_back(component);
    //         }
    //         delimiter = (delimiter == '{') ? '}' : '{';
    //     }
    // }

    // // Update the original vector with divided components
    // result = dividedComponents;
    return (result);
}

Configuration ConfigFile::GetConfig(std::string hostPort, std::string serverName)
{
	std::vector<Configuration>::iterator it = _mapConfigs[hostPort].begin();
	Configuration defaultConfig;
	for ( ; it != _mapConfigs[hostPort].end(); it++)
	{
		if (it->GetServerName().empty())
			defaultConfig = *it;
		if (it->GetServerName() == serverName)
			return *it;
	}
	return defaultConfig;
}

std::map<std::string, std::vector<Configuration> > ConfigFile::GetMapConfig(){
	return _mapConfigs;
}

std::vector<Configuration> ConfigFile::GetConfigs()
{
	return _configs;
}