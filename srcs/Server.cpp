#include "WebServer.h"


Server::Server(unsigned int port, std::string host): _port(port), _host(host)
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
    
}
