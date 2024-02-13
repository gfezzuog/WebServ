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
    Configuration *_config;
public:
    Server(unsigned int port, std::string host, Configuration *config);
    ~Server();

    fd_set _readfds;
    void connect();
    void disconnect();
    int GetSocketfd() {return _socketfd;}
    int GetPort() {return _port;}
    int Getmaxfd() {return _maxfd;}
    fd_set *GetFDS() {return &_readfds;}
    std::string GetHost() {return _host;}
    void initialize()
    {
        FD_ZERO(&_readfds);
    }
    std::string GetHostPort()
    {
        std::string hostPort = _host;
        hostPort.append(":");
        return hostPort.append(to_string(_port));
    }
    fd_set *Getreadfds() {return &_readfds;}
    sockaddr_in* GetSocketAddr () {return &_sockAddress;}
    Configuration *GetConfig();
    Configuration GetConf();
};
