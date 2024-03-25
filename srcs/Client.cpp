#include "../incs/WebServer.h"

Clients::Clients() { return ; }

Clients::~Clients()
{
}

// Clients::client_data *Clients::C_structGet(int fd) {
// 	int n = 0;
// 	std::vector<client_data>::iterator i = _clients.begin();

// 	for ( ; i != _clients.end(); i++, n++)
// 		if ((*i).fd == fd)
// 			return &(*i);
// 	return NULL;
// }

Clients::client_data* Clients::C_structGet(int fd) {
    for (size_t n = 0; n < _clients.size(); ++n) {
        if (_clients[n].fd == fd) {
            return &_clients[n];
        }
    }
    return nullptr;
}


int Clients::C_structAdd(int fd, int evIdent) {
	if (fd < 1) return -1;
	int flags = fcntl(fd, F_GETFL, 0);
	assert(flags >= 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	_clients.push_back(client_data(fd, evIdent));
	return 0;
}

// int Clients::C_structDel(int fd) {
// 	if (fd < 1) return -1;
// 	client_data* cd = C_structGet(fd);
// 	if (!cd) return -1;
// 	std::vector<client_data>::iterator it = _clients.begin();
// 	for ( ; (*it).fd != (*cd).fd; it++) ;
// 	_clients.erase(it);
// 	return close(fd);
// }

int Clients::C_structDel(int fd) {
    if (fd < 1) return -1;
    client_data* cd = C_structGet(fd);
    if (!cd) return -1;
    for (std::vector<client_data>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
        if ((*it).fd == (*cd).fd) {
            _clients.erase(it);
            break;
        }
    }
    return close(fd);
}