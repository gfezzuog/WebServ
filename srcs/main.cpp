#include "../incl/WebServer.h"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./webserver <config_file>" << std::endl;
		return 1;
	}
	ConfigFile config(argv[1]);
	return 0;
}