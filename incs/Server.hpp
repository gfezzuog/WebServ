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
    struct sockaddr_in _sockAddress;
    int _maxfd;
    int _servconn;
    Configuration *_config;
public:
    Server(unsigned int port, std::string host, Configuration *config);
    ~Server();

    fd_set _readfds;
    void connect();
    void disconnect();
    int GetSocketfd() {return _socketfd;}
    int GetPort() {return _port;}
    fd_set *GetFDS() {return &_readfds;}
    std::string GetHost() {return _host;}
    std::string GetHostPort()
    {
        std::string hostPort = _host;
        hostPort.append(":");
        return hostPort.append(to_string(_port));
    }
    sockaddr_in* GetSocketAddr () {return &_sockAddress;}
    int configureServer();
    Configuration *GetConfig();
    Configuration GetConf();
    int GetServConn(){return _servconn;}
    void SetServConn(int fd){
        _servconn = fd;
    }
    void closing();

};
