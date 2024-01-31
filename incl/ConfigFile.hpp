#pragma once

#include "WebServer.h"

class ConfigFile{
		private:
			std::ifstream _file;
			std::string _content;
			std::vector<std::string> _configString;
			std::vector<Configuration> _configs;

			std::vector<std::string> splitString(std::string input);
		public:
			ConfigFile(std::string inputfile);
			~ConfigFile();

};