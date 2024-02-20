#include "../incs/WebServer.h"

bool run = 1;
fd_set _readfds;

static void signal_handler(int i)
{
	if (i == SIGINT)
		run = 0;
}

static int findServerByFD(std::vector<Server *> servers, int ident)
{
	int i = 0;
	std::vector<Server *>::iterator it = servers.begin();
	for (; it != servers.end(); it++, i++)
		if ((**it).GetSocketfd() == ident)
			return i;
	return -1;
}

std::vector<Server *> initServers(std::map<std::string, std::vector<Configuration> > portConfigs)
{
	std::map<std::string, std::vector<Configuration> >::iterator it = portConfigs.begin();
	std::vector<Server *> servers;
	int i = 0;

	for (; it != portConfigs.end(); it++)
	{
		Server *s = new Server((*it).second[0].GetPort(), (*it).second[0].GetHost(), &((*it).second[0]));
		try
		{
			s->connect();
			FD_SET(s->GetSocketfd(), &_readfds);
			
		}
		catch (const OpenSocketException &e)
		{
			std::cout << RED << e.what() << RESET << std::endl;
		}
		std::cout<< "server bananioooooo"<<std::endl;
		servers.push_back(s);
		i++;
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
	std::cout<<"????????????????"<<servers[0]->GetConfig()<<std::endl;
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
						std::cout<< "connection = " << connection << std::endl;
						if (connection < 0)
						{
							std::cerr << "Failed to accept connection. errno: " << errno << std::endl;
							continue;
						}
						else
						{
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
								if (bytes_read <= 0)
								{
									// Gestisci errore o connessione chiusa dal client
									close(connection);
									FD_CLR(connection, &_readfds);
									continue;
								}
								int index = findServerByFD(servers, clients.Get_conn(servers[i]->GetSocketfd())->fd);
								Configuration config;
								RequestHeader reqHeader(buffer, bytes_read + 1);
								try
								{
									std::cout<<"server index host port = "<<std::endl;
									std::cout<< servers[index]->GetHostPort()<<std::endl;
									config = cf.GetConfig((*servers[index]).GetHostPort(), reqHeader.GetHost());
									if(config.isEmpty())
									{
										throw std::exception();
									}
								}
								catch(const std::out_of_range &e)
								{	
									continue;
								}
								catch(const std::exception &e)
								{
									std::cout<< "Error: no vaiable configuration found"<<std::endl;
									continue;
								}
								std::cout << "Request method: " << reqHeader.GetMethod() << std::endl;
								std::cout << "Request path: " << reqHeader.GetPath() << std::endl;
								std::cout<<index<<std::endl;
								ResponseHeader resHeader = ResponseHeader(servers[index], &reqHeader, &config);
								std::string response;
								try{
									if(reqHeader.GetMethod() == "POST" && reqHeader.GetBody().length() > config.GetLimitSizeBody()){
										resHeader = ResponseHeader(NULL, std::make_pair("413", config.GetErrorPath("413")));
										throw ServerException(resHeader.getError().first,
											resHeader.makeResponse(std::atoi(resHeader.getError().first.c_str())), connection);
									}
									else {
										if(!config.GetRedirectionCode())
											response.append(resHeader.makeResponse(200));
										else{
											response.append(resHeader.makeResponse(config.GetRedirectionCode()));
											std::string redir("Location");
											response.append(config.GetRedirectionUrl());
											response.append("\r\n");
											response.insert(response.find('\n') + 1, redir);
										}
										if(!resHeader.getError().first.empty()){
											resHeader = ResponseHeader(NULL, resHeader.getError());
											throw ServerException(resHeader.getError().first,
												resHeader.makeResponse(std::atoi(resHeader.getError().first.c_str())), connection);
										}
									}
								}
								catch (const ServerException &e){
									response = e.what();
								}
								std::string chunck;
								int data_sent = 0;
								
								do{
									chunck = response.substr(0, response.size());
									data_sent = send(connection, chunck.c_str(), chunck.size(), 0);
									if(data_sent < 0)
										break;
									response.substr(data_sent);
								} while(response.size());
								index = findServerByFD(servers, connection);
								FD_SET(sockfd, &_readfds);
								// std::cout<<"RETURN FROM CON_DELETE: "<<clients.conn_delete(servers[index]->GetSocketfd())<<std::endl;
								// clients.conn_delete(clients.Get_conn(servers[index]->GetSocketfd())->fd);
								usleep(100);
								// if(reqHeader.GetMethod() == "GET")
								// {
								// 	response = resHeader.makeResponse(200);
								// }
								// else if (reqHeader.GetMethod() == "POST")
								// {
								// 	response = resHeader.makeResponse(413);
								// }
								// else
								// 	response = resHeader.makeResponse(405);
								// ssize_t byte_sent = send(connection, response.c_str(), response.length(), 0);
								// if (byte_sent < 0)
								// 	std::cerr << "madonna laida"<<std::endl;
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
			break;
		}
	}
		std::cout << RED << "Cribbio EXIT" << RESET << std::endl;
		for (size_t i = 0; i < servers.size(); ++i)
		{
			servers[i]->disconnect();
			delete servers[i];
		}
		servers.clear();

		return 0;
	
}
