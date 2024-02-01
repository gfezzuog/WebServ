#pragma once

#include "WebServer.h"

#define DEFAULT_CONF = "default_config_file.conf" 

class ConfigFile{
		private:
			std::ifstream _file;
			std::string _content;
			std::vector<std::string> _configString;
			std::vector<Configuration> _configs;
			std::map<std::string, std::vector<Configuration>> _mapConfigs;
			void setMapConfigs();
			std::vector<std::string> splitString(std::string input);
		public:
			ConfigFile(std::string inputfile);
			~ConfigFile();
			Configuration getConfig(std::string host, std::string servername);
			std::map<std::string, std::vector<Configuration>> getMapConfig();
			std::vector<Configuration> getConfigs();
};