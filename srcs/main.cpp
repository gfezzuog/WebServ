#include "../incl/WebServer.h"

bool run = 1;
fd_set _readfds;

static void signal_handler(int i)
{
	if (i == SIGINT)
		run = 0;
}

std::vector<Server *> initServers(std::map<std::string, std::vector<Configuration> > portConfigs)
{
	std::map<std::string, std::vector<Configuration> >::iterator it = portConfigs.begin();
	std::vector<Server *> servers;

	for (; it != portConfigs.end(); it++)
	{
		Server *s = new Server((*it).second[0].GetPort(), (*it).second[0].GetHost(), &((*it).second[0]));
		try
		{
			// s->configureServer();
			s->connect();
			FD_SET(s->GetSocketfd(), &_readfds);
		}
		catch (const OpenSocketException &e)
		{
			std::cout << RED << e.what() << RESET << std::endl;
		}
		servers.push_back(s);
	}
	return servers;
}

void printReadFDs(fd_set readfds) {
	std::cout << "File descriptors ready for reading: "<<std::endl;
	for (int fd = 0; fd < FD_SETSIZE; ++fd) {
		if (FD_ISSET(fd, &readfds)) {
			std::cout << fd << " PRONTO"<<std::endl;
		}
	}
	std::cout << std::endl;
}
int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		std::cerr << RED << "Error: Wrong numbers of arguments" << RESET << std::endl;
		return 1;
	}
	FD_ZERO(&_readfds);
	ConfigFile cf(argc == 1 ? "default_config_file.conf" : argv[1]);
	signal(SIGINT, signal_handler);
	execAutoindex();

	std::vector<Server *> servers = initServers(cf.GetMapConfig());
	std::cout << "Servers created" << std::endl;
	printReadFDs(_readfds);
	Clients clients;

	while (run)
	{
		std::cout << "run begin" << std::endl;
		int maxfd = -1;
		for (size_t i = 0; i < servers.size(); ++i)
		{
			int sockfd = servers[i]->GetSocketfd();
			std::cout << sockfd << " il file descriptor è questo" << std::endl;
			if (sockfd > maxfd)
				maxfd = sockfd;
		}
		std::cout << "max fd done " << maxfd << std::endl;
		for (size_t i = 0; i < servers.size(); ++i)
		{
			FD_SET(servers[i]->GetSocketfd(), &_readfds);
			if (FD_ISSET(servers[i]->GetSocketfd(), &_readfds))
				std::cout << "FD_SET DONE " << i << std::endl;
			else
				std::cout << "error???????" << std::endl;
		}

		int num_ready = select(maxfd + 1, &_readfds, NULL, NULL, NULL);
		std::cout << "select return = " << num_ready << std::endl;

		if (num_ready > 0)
		{
			for (size_t i = 0; i < servers.size(); ++i)
        	{
				int sockfd = servers[i]->GetSocketfd();
				if (FD_ISSET(sockfd, &_readfds))
				{
					if (sockfd == servers[i]->GetSocketfd())
					{
						struct sockaddr_in clientAddr;
						socklen_t clientAddrLen = sizeof(clientAddr);
						int connection = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
						if (connection < 0)
						{
							std::cerr << "Failed to accept connection. errno: " << errno << std::endl;
							continue;
						}
						else
						{
							std::cout << "Accepted new connection" << std::endl;
							if (clients.conn_add(sockfd))
							{
								ResponseHeader resHeader(NULL, std::make_pair("500", DEFAULT_ERROR_PATH));
								throw ServerException("500", resHeader.makeResponse(500), connection);
							}
							else
							{
								FD_SET(connection, &_readfds);

								// Leggi i dati inviati dal client
								char buffer[MAX_BUFFER_SIZE];
								ssize_t bytes_read = recv(connection, buffer, MAX_BUFFER_SIZE, 0);
								std::cout << "buffer: " << buffer << "read: " << bytes_read << std::endl; 
								if (bytes_read <= 0)
								{
									// Gestisci errore o connessione chiusa dal client
									std::cerr << "Error reading request from client." << std::endl;
									close(connection);
									FD_CLR(connection, &_readfds);
									continue;
								}
								RequestHeader reqHeader(buffer, bytes_read);

								// Ora puoi utilizzare le informazioni sulla richiesta per elaborarla
								Configuration *banani = servers[i]->GetConfig();
								if(banani == NULL)
								{
									std::cout<<"banani di merda"<<std::endl;
									return(1);
								}
								std::cout << "Request method: " << reqHeader.GetMethod() << std::endl;
								std::cout << "Request URL: " << reqHeader.GetPath() << std::endl;
								ResponseHeader resHeader(servers[i], &reqHeader, banani);
								std::string response;
								std::cout << "BANANI"<<std::endl;
								if(reqHeader.GetMethod() == "GET")
								{
									response = resHeader.makeResponse(200);
									std::cout<<"request is GET"<<std::endl;
								}
								else
								{
									response = resHeader.makeResponse(405);
									std::cout<<"response 405"<<std::endl;
								}
								std::cout << "everything done tipe so send byte"<<std::endl;
								ssize_t byte_sent = send(connection, response.c_str(), response.length(), 0);
								std::cout <<"data sent"<<std::endl;
								if (byte_sent < 0)
									std::cerr << "madonna laida"<<std::endl;
							}
						}
					}
				}
			}
		}
		else if (num_ready == 0)
		{
			continue;
		}
		else
		{
			std::cerr << "Error in select" << std::endl;
			break;
		}
	}
		std::cout << RED << "EXIT" << RESET << std::endl;
		for (size_t i = 0; i < servers.size(); ++i)
		{
			servers[i]->disconnect();
			delete servers[i];
		}
		servers.clear();

		return 0;
	
}
