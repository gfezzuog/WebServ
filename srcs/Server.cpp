#include "WebServer.h"


Server::Server(unsigned int port, std::string host, Configuration *config): _port(port), _host(host), _config(config)
{}


Server::~Server()
{
}

void Server::connect(int kQueue)
{
    _socketfd = socket(AF_INET, SOCK_STREAM, 0);
    //if _socketfd = -1 EXCEPTION
    _sockAddress.sin_family = AF_INET;
    //if _host == NULL EXCEPTION
    bool b = 1;
    setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &b, sizeof(int));
    setsockopt(_sockfd, SOL_SOCKET, SO_NOSIGPIPE, &b, sizeof(int));
    _sockAddress.sin_port =  htons(_port);
    //if bind (sock_fd, (struct sockaddr*) &_sockAddr, sizeof(_sockAddr)) < 0
    // OperSocketException
    // if(listen(_sockfd, 10) < 0)
    // Open SocketExcpetion
    EV_SET (&_evSet, _sockfd, EVFILT_READ, EV_ADD, 0, 0, NULL);
    int n_fds = select(maxfd + 1, &readfds, nullptr, nullptr, nullptr);
    std::cout<< "Connected to host: " << _host << " and port: "<< _port <<std::endl;
}

void Server::disconnect()
{
    close(_sockfd);
}

Configuration Server::getConfig()
{
    return *_config;
}
