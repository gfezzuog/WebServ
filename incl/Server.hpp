#pragma once

#include "WebServer.h"

template<typename T>
std::string to_string(const T & value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

class Configuration;

class Server
{
private:
    int _port;
    int _socketfd;
    std::string _host;
    sockaddr_in _sockAddress;
    int _maxfd;
    fd_set _readfds;
    Configuration *_config;
public:
    Server(unsigned int port, std::string host, Configuration *config);
    ~Server();

    void connect();
    void disconnect();
    int GetSocketfd() {return _socketfd;}
    int GetPort() {return _port;}
    int Getmaxfd() {return _maxfd;}
    fd_set *GetFDS() {return &_readfds;}
    std::string GetHost() {return _host;}
    std::string GetHostPort()
    {
        std::string hostPort = _host;
        hostPort.append(":");
        return hostPort.append(to_string(_port));
    }
    sockaddr_in* GetSocketAddr () {return &_sockAddress;}
    Configuration GetConfig();
};
