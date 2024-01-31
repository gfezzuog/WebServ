#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>


class FileReader {
private:
    std::string inputFileName;
    std::string content;
public:
    FileReader(const std::string& inputFileName);
	void printVector(const std::vector<std::string>& strings);
	std::vector<std::string> splitString(const std::string& input);
	std::string SaveContent();
};