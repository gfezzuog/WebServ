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
};