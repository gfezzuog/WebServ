#pragma once

#include "WebServer.h"

class Server
{
private:
    int _port;
    int _socketfd;
    std::string host;
    sockaddr_in _sockAddress;
    struct kevent _evSet;
    //Configuration *_config;
    
    
public:
    Server(unsigne int port, std::string host); //Configuration *config
    ~Server();

    void connect(int kQueue);
    void disconnect();
    int getSocketfd() {return _sockfd;}
    int getPort() {return _port;}
    std::string getHost() {return _host;}
    std::string getHostPort()
    {
        std::string hostPort = _host;
        hostPort.append(":");
        return hostPort.append(std::to_string(_port));
    }
    sockaddr_in* getSocketAddr () {return *_sockAddress};
    struct kevent* getEvSet() {return *_evSet;}
    //Config *GetConfig() {return *_config;}
};
