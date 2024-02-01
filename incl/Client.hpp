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
			c_data(int conn) : fd(conn){return;}
		};

		std::vector<c_data> _clients;

	public:
		Clients();
		~Clients();

		c_data *get_conn(int fd);
		int conn_add(int fd);
		int conn_delete(int fd);
		
};