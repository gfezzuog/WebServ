#pragma once

#include "WebServer.h"

#define DEFAULT_CONF = "default_config_file.conf" 

class ConfigFile{
		private:
			std::ifstream _file;
			std::string _content;
			std::vector<std::string> _configString;
			std::vector<Configuration> _configs;
			//map <string, config vector> _mapConfigs
			//setmapConfigs
			std::vector<std::string> splitString(std::string input);
		public:
			ConfigFile(std::string inputfile);
			~ConfigFile();
			/*
			Configuration getConfig by port and server name
			getMapConfig
			*/
			std::vector<Configuration> getConfigs();
};