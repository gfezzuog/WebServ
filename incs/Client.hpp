#pragma once

#include "WebServer.h"

class Clients
{
	private:
		struct c_data
		{
			int fd;
			int sockfd;
			c_data(int conn, int socketfd) : fd(conn), sockfd(socketfd){return;}
		};

		std::vector<c_data> _clients;

	public:
		Clients();
		~Clients();

		c_data *Get_conn(int fd);
		int conn_add(int fd, int socketfd);
		int conn_delete(int fd);
		
};