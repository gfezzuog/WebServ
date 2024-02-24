#include "../incs/WebServer.h"

bool run = 1;
fd_set _readfds;

static void signal_handler(int i)
{
	if (i == SIGINT)
		run = 0;
}

static int findServerByFD(std::vector<Server *> servers, int fd)
{
	int i = 0;
	std::vector<Server *>::iterator it = servers.begin();
	for (; it != servers.end(); it++, i++)
		if ((**it).GetServConn() == fd)
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
		}
		catch (const OpenSocketException &e)
		{
			std::cout << RED << e.what() << RESET << std::endl;
		}
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

// int main(int argc, char *argv[]){
// 	if (argc > 2)
//  	{
//  		std::cerr << RED << "Error: Wrong numbers of arguments" << RESET << std::endl;
//  		return 1;
//  	}

// }

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
	Clients clients;

	int maxfd = -1;
	while (run)
	{
		FD_ZERO(&_readfds);
		for (size_t i = 0; i < servers.size(); ++i)
		{
			int sockfd = servers[i]->GetSocketfd();
			FD_SET(sockfd, &_readfds);
			if (sockfd > maxfd)
				maxfd = sockfd;
		}
		// for (size_t i = 0; i < servers.size(); ++i)
		int num_ready = select(maxfd + 1, &_readfds, NULL, NULL, NULL);

		if (num_ready > 0)
		{
			for (size_t i = 0; i < servers.size(); i++)
        	{
				int sockfd = servers[i]->GetSocketfd();
				std::cout << "Sockfd at " << i << " dio porco "<< sockfd << std::endl;
				if (FD_ISSET(sockfd, &_readfds))
				{
						std::cout << "ho finito le idee"<< std::endl;
						struct sockaddr_in clientAddr;
						socklen_t clientAddrLen = sizeof(clientAddr);
						int connection = accept(sockfd, (struct sockaddr *)&clientAddr, &clientAddrLen);
						std::cout<<"accept return"<<connection<<std::endl;
						// for(size_t o = 0; o < servers.size(); o++)
						// 	std::cout << "sockfd & servers[i]->_socketfd "<< sockfd << " "<< servers[o]->GetSocketfd()<<std::endl;
						std::cout<<" 1 "<<std::endl;
						FD_SET(connection, &_readfds);
						printReadFDs(_readfds);

						if (connection < 0)
						{
							std::cerr << "Failed to accept connection. errno: " << errno << std::endl;
							continue;
						}
						else
						{
							servers[i]->SetServConn(connection);
							int index = findServerByFD(servers, connection);
							if (clients.conn_add(connection))
							{
								ResponseHeader resHeader(NULL, std::make_pair("500", DEFAULT_ERROR_PATH));
								throw ServerException("500", resHeader.makeResponse(500), servers[i]->GetServConn());
							}
							else
							{
								// Leggi i dati inviati dal client
								char buffer[MAX_BUFFER_SIZE];
								ssize_t bytes_read = recv(servers[index]->GetSocketfd(), buffer, MAX_BUFFER_SIZE, 0);
								std::cout << "recv: " << bytes_read << std::endl;
								if (bytes_read <= 0)
								{
									// Gestisci errore o connessione chiusa dal client
								std::cout << "PORCO DIOOOOOOOOOOOOOOOOOOO\n";
									close(connection);
									FD_CLR(connection, &_readfds);
									continue;

								}
								index = findServerByFD(servers, connection);
								Configuration config;
								RequestHeader reqHeader(buffer, bytes_read + 1);
								try
								{
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
									std::cout<< "Error: no viable configuration found"<<std::endl;
									continue;
								}
								ResponseHeader resHeader = ResponseHeader(servers[index], &reqHeader, &config);
								std::cout << "la madonna con la cresta " << resHeader.getPath() << std::endl;
								std::string response;
								try{
									if(reqHeader.GetMethod() == "POST" && reqHeader.GetBody().length() > config.GetLimitSizeBody()){
										resHeader = ResponseHeader(NULL, std::make_pair("413", config.GetErrorPath("413")));
										throw ServerException(resHeader.getError().first,
											resHeader.makeResponse(std::atoi(resHeader.getError().first.c_str())), connection);
									}
									else {
										if(!config.GetRedirectionCode())
                                        {
											response.append(resHeader.makeResponse(200));
                                        }
                                        else{
                                            std::cout<<"Make response called\n";
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
									std::cout<<"checking here"<<std::endl;
								}
									// servers[i]->closing();
								std::string chunck;
								int data_sent = 0;
								
								do{
									std::cout << "stepbro"<< std::endl;
									chunck = response.substr(0, 35000);
									data_sent = send(connection, chunck.c_str(), chunck.size(), 0);
									if(data_sent < 0)
										break;
									response.substr(data_sent);
								} while(response.size());
								FD_CLR(connection, &_readfds);
								std::cout<< " 2 "<<std::endl;
								printReadFDs(_readfds);
								index = findServerByFD(servers, connection);
								close(connection);
								clients.conn_delete(servers[index]->GetServConn());
								// servers[index]->SetServConn(0);
								usleep(1000);
								// i = findServerByFD(servers, connection);
								// FD_SET(sockfd, &_readfds);
								// std::cout<<"RETURN FROM CON_DELETE: "<<clients.conn_delete(servers[index]->GetSocketfd())<<std::endl;
								// clients.conn_delete(clients.Get_conn(servers[index]->GetSocketfd())->fd);
								//usleep(100);
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
						std::cout << "i'm in "<< std::endl;					
						}
				}
			
		}
		// else if (num_ready == 0)
		// {
		// 	continue;
		// }
		// else
		// {
		// 	break;
		// }
		std::cout << "aaaaaaaaaaaaaaa" << std::endl;
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
