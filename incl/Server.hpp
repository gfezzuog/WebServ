#pragma once

#include "WebServer.h"

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

    void connect(int kQueue);
    void disconnect();
    int getSocketfd() {return _socketfd;}
    int getPort() {return _port;}
    std::string getHost() {return _host;}
    std::string getHostPort()
    {
        std::string hostPort = _host;
        hostPort.append(":");
        return hostPort.append(std::to_string(_port));
    }
    sockaddr_in* getSocketAddr () {return *_sockAddress};
    Config *GetConfig() {return *_config;}
};
