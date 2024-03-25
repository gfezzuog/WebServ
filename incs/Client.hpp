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

	client_data *C_structGet(int fd);
	int C_structAdd(int fd, int evIdent);
	int C_structDel(int fd);
};
