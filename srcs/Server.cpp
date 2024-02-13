#include "../incl/WebServer.h"


Server::Server(unsigned int port, std::string host, Configuration *config): _port(port), _host(host), _config(config)
{
}


Server::~Server()
{
}

void Server::connect() {
    _socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_socketfd == -1) {
        throw OpenSocketException(std::string("Failed to create socket. errno: ").append(std::to_string(errno)));
    }
    _sockAddress.sin_family = AF_INET;
    if ((_sockAddress.sin_addr.s_addr = inet_addr(_host.c_str())) == INADDR_NONE) {
        throw OpenSocketException(std::string("Host ").append(std::string(_host)).append(" is not valid."));
    }
    bool b = 1;
    setsockopt(_socketfd, SOL_SOCKET, SO_REUSEADDR, &b, sizeof(int));
    setsockopt(_socketfd, SOL_SOCKET, SO_NOSIGPIPE, &b, sizeof(int));
    _sockAddress.sin_port = htons(_port);
    if (bind(_socketfd, (struct sockaddr*)&_sockAddress, sizeof(_sockAddress)) < 0) {
        throw OpenSocketException(std::string("Failed to bind port ").append(std::to_string(_port)).append(". errno: ").append(std::to_string(errno)));
    }
    if (listen(_socketfd, 10) < 0) {
        throw OpenSocketException(std::string("Failed to listen on socket. errno: ").append(std::to_string(errno)));
    }

    FD_SET(_socketfd, &_readfds);

    std::cout << GREEN << "Connected to host: " << BLUE << _host << GREEN << " and port: " << BLUE << _port << RESET << std::endl;
}

void Server::disconnect()
{

    close(_socketfd);
    FD_CLR(_socketfd, &_readfds);

}

Configuration *Server::GetConfig()
{
    return _config;
}

Configuration Server::GetConf()
{
    return *_config;
}