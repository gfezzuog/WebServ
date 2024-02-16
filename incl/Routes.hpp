#pragma once

#include "WebServer.h"

class ConfigurationRoute{
	private:
		std::string _path;
		std::map<std::string, std::string> _map;
		void setMap(std::string config);
	public:
		ConfigurationRoute();
		ConfigurationRoute(std::string config, std::string path);
		~ConfigurationRoute();
		std::string GetPath();
		std::string GetRoot();
		std::vector<std::string> GetCGIPath();
		std::string GetUploadPath();
		bool GetAutoIndex();
		std::string GetMethods();
		std::string GetIndex();
		void printConfigurationroute() {
			 std::cout << "Configuration Route Details:" << std::endl;
		std::cout << "Path: " << _path << std::endl;

		std::cout << "Map:" << std::endl;
		for (const auto& pair : _map) {
				std::cout << pair.first << " : " << pair.second << std::endl;
		}
			
		std::cout << "Root: " << GetRoot() << std::endl;
		std::cout << "CGI Paths:" << std::endl;
		for (const auto& cgiPath : GetCGIPath()) {
				std::cout << cgiPath << std::endl;
		}

		std::cout << "Upload Path: " << GetUploadPath() << std::endl;
		std::cout << "AutoIndex: " << (GetAutoIndex() ? "Enabled" : "Disabled") << std::endl;
		std::cout << "Allowed Methods: " << GetMethods() << std::endl;
		std::cout << "Index: " << GetIndex() << std::endl;

		std::cout << "End of Configuration Route" << std::endl;
	}
};