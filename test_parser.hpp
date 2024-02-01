#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

class Configuration
{
	public:

	std::string host;
	std::string servername;
	std::string port;
	std::string password;
	std::string mode;
	std::string username;
};

class FileReader {
private:
    std::string inputFileName;
    std::string content;

	Configuration getConfig(std::string host, std::string servername);
	std::map<std::string, std::vector<Configuration>> getMapConfig();
	std::vector<Configuration> getConfigs();

public:
    std::vector<Configuration> DivideServers(std::vector<std::string> servers);
    FileReader(const std::string& inputFileName);
	void printVector(const std::vector<std::string>& strings);
	std::vector<std::string> splitString(const std::string& input);
	std::string SaveContent();
};