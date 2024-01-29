#pragma once

#include "WebServer.h"

class Clients
{
	private:
		struct c_data
		{
			int fd;
			int _maxfd;
			fd_set _readfds;
			Configuration *config;
			c_data(int conn, int maxfd, fd_set readfds) : fd(conn), _maxfd(maxfd), _readfds(readfds){return;}
		};

		std::vector<c_data> _clients;

	public:
		Clients();
		~Clients();

		c_data *get_conn(int fd);
		int conn_add(int fd, int _maxfd, fd_set _readsfds);
		int conn_delete(int fd);
		
};