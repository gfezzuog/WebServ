#ifndef WEBSERVER_H
#define WEBSERVER_H

# define RESET "\e[0m"
# define LLONG_MAX = 9223372036854775807

#define MAX_EVENTS 1024
// #define DEFAUL_ERROR_PATH ""
// #define PYTHON3PATH ""
// #define AUTOINDEXPATH ""

# include <iomanip>
# include <fcntl.h>
# include <cstdlib>
# include <ctype.h>
# include <sys/stat.h>
# include <sys/socket.h>
//# include <sys/event.h>
# include <arpa/inet.h>
# include <netinet/in.h>
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <ctime>
# include <string>
# include <iostream>
# include <fstream>
# include <sstream>
# include <vector>
# include <map>
# include <algorithm>
# include "Server.hpp"
# include "Config.hpp"
# include "ConfigFile.hpp"
# include "OpenSocketException.hpp"

#endif