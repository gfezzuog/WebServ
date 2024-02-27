#pragma once

#include "WebServer.h"

class Clients
{
private:
	struct client_data
	{
		int fd;
		int evIdent;
		Configuration *config;
		client_data(int conn, int evId) : fd(conn), evIdent(evId){ return; }
	} ;

	std::vector<client_data> _clients;
public:
	Clients();
	~Clients();

	client_data *Get_conn(int fd);
	int Conn_add(int fd, int evIdent);
	int Conn_delete(int fd);
};
