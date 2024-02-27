#pragma once

#include "WebServer.h"

template<typename T>
std::string to_string(const T & value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

class Configuration;

class Server {
	private:
		int _sockfd;
		int _port;
		std::string _host;
		sockaddr_in _sockAddr;
		struct kevent _evSet;
		Configuration *_config;
	public:
		Server(unsigned int port, std::string host, Configuration *config);
		~Server();

		void connect(int kQueue);
		void disconnect();
		int GetSocketfd() { return _sockfd; }
		int GetPort() { return _port; }
		std::string GetHost() { return _host; }
		std::string GetHostPort()
		{
			std::string hostPort = _host;
			hostPort.append(":");
			return hostPort.append(std::to_string(_port));
		}
		sockaddr_in* GetSockAddr() { return &_sockAddr; };
		struct kevent* GetEvSet() { return &_evSet; };
		Configuration GetConfig();
};
