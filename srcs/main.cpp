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
		if ((**it).GetSocketfd() == fd)
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

int main(int argc, char *argv[]){
	if (argc > 2)
 	{
 		std::cerr << RED << "Error: Wrong numbers of arguments" << RESET << std::endl;
 		return 1;
 	}
	ConfigFile cf(argc == 1 ? DEFAUL_PATH : argv[1]);
	signal(SIGINT, signal_handler);
	execAutoindex();
	
	std::vector<Server *> servers = initServers(cf.GetMapConfig());
	Clients clients;

	int maxfd = 0;
	int sockfd = 0;
	fd_set _writefds;
	
	while (run)
	{
		int connection = 0;
		FD_ZERO(&_readfds);
		FD_ZERO(&_writefds);
		for(size_t i = 0; i < servers.size(); i++)
		{
			sockfd = servers[i]->GetSocketfd();
			FD_SET(sockfd, &_readfds);
			if(sockfd > maxfd)
				maxfd = sockfd;
		}
		printReadFDs(_readfds);
		int fdReady = 0;
		fdReady = select(maxfd + 1, &_readfds, NULL, NULL, NULL);
		std::cout << "select what the actuall fuck: " << fdReady<< std::endl;
		printReadFDs(_readfds);
		std::string buffStr;
		for(int i = 0; i < (int)servers.size(); i++)
		{
			std::cout<<"inside if: "<< i<<std::endl;
			printReadFDs(_readfds);
			sockfd = servers[i]->GetSocketfd();
			int index = findServerByFD(servers, sockfd);
			std::cout << "index: " << index << " connection: "<< connection<< std::endl;
			if(connection == 0)
			{
				// struct sockaddr_in clientAddr;
				// socklen_t clientAddrLen = sizeof(clientAddr);
				// connection = accept(servers[index]->GetSocketfd(), (struct sockaddr *)&clientAddr, &clientAddrLen);
				ssize_t addresslen = sizeof(sockaddr);
				std::cout << "why???? " << std::endl;
				connection = accept(sockfd, (struct sockaddr *)(*servers[index]).GetSocketAddr(), (socklen_t *)&addresslen);
				std::cout << "Connection: " << connection << std::endl;
				try
				{
					if(clients.conn_add(connection, sockfd))
					{
						ResponseHeader resHeader(NULL, std::make_pair("500", DEFAULT_ERROR_PATH));
						throw ServerException("500", resHeader.makeResponse(500), connection);
					}
					else
					{
						FD_SET(connection, &_writefds);
						if(connection > maxfd)
							maxfd = connection;
						printReadFDs(_readfds);
						if (connection < 0)
						{
							std::cerr << "Failed to accept connection. errno: " << errno << std::endl;
							continue;
						}
						fdReady = select(maxfd + 1, &_readfds, &_writefds, NULL, NULL);
					}
				}
				catch(const ServerException &e)
				{
					e.what();
				}
			}
			else
			{
				Configuration config;
				char buffer[8192];
				index = findServerByFD(servers, clients.Get_conn(connection)->sockfd);
				size_t total_bytes = 0;
				int bytes_read = 0;
				std::cout << BLUE << "Before do while. Connection = " << connection << RESET << std::endl;
				do
				{
					std::cout << BLUE << "Inside do while. Connection = " << connection << std::endl;
					bytes_read = recv(connection, buffer, 8192, 0);
					std::cout << RED << "recv: " << bytes_read << RESET << std::endl;
					total_bytes += bytes_read;
					std::cout << buffer << std::endl;
					if(bytes_read > 0)
						buffStr.append(buffer, bytes_read);
					usleep(100);
				}while(bytes_read > 0);
				std::cout << BLUE << "After do while. Connection = " << connection << RESET << std::endl;
				std::cout << "BuffStr:" << std::endl << buffStr << std::endl;
				RequestHeader reqHeader = RequestHeader(buffStr, total_bytes + 1);
				try
				{
					std::cout << BLUE << "Inside try config = cf. Host Port: " << (*servers[index]).GetHostPort() << "Get host: " << reqHeader.GetHost() << RESET <<std::endl;
					config = cf.GetConfig((*servers[index]).GetHostPort(), reqHeader.GetHost());
					if(config.isEmpty())
						throw std::exception();
				}
				catch(const std::out_of_range& e)
				{
					std::cout << RED << "PORCO DIO" << RESET << std::endl;
					continue;
				}
				catch(const std::exception &e)
				{
					std::cout << RED << "Error: no vaiable configuration" << RESET << std::endl;
					continue;
				}
				ResponseHeader resHeader = ResponseHeader(servers[index], &reqHeader, &config);
				std::string response;
				std::cout << GREEN << "after resHeader" <<RESET <<std::endl;
				try
				{
					if(reqHeader.GetMethod() == "POST" && reqHeader.GetBody().length() > config.GetLimitSizeBody())
					{
						resHeader = ResponseHeader(NULL, std::make_pair("413", config.GetErrorPath("413")));
						throw ServerException(resHeader.getError().first,
							resHeader.makeResponse(std::atoi(resHeader.getError().first.c_str())),
							connection);
					}
					else
					{
						if(!config.GetRedirectionCode())
							response.append(resHeader.makeResponse(200));
						else
						{
							response.append(resHeader.makeResponse(config.GetRedirectionCode()));
							std::string redir("Location: ");
							redir.append(config.GetRedirectionUrl());
							redir.append("\r\n");
							response.insert(response.find('\n') + 1, redir);
						}
					}
				}
				catch(const ServerException &e)
				{
					response = e.what();
				}
				std::string chunck;
				int data_sent = 0;
				std::cout << BLUE << "Before send" << RESET <<std::endl;
				do
				{
					chunck = response.substr(0, 35000);
					data_sent = send(connection, chunck.c_str(), chunck.size(), 0);
					if(data_sent < 0)
						break;
					response = response.substr(data_sent);
				} while (response.size());
				std::cout << BLUE << "After send" << RESET <<std::endl;

				buffStr.clear();
				index = findServerByFD(servers, clients.Get_conn(connection)->sockfd);
				FD_CLR(connection, &_writefds);
				FD_CLR(sockfd, &_readfds);
				clients.conn_delete(connection);
				close(connection);
				std::cout << GREEN << "After clearing connections" << RESET << std::endl;
				usleep(100);
			}
		}
	}
	std::cout << RED << "EXIT" << RESET << std::endl;
	FD_ZERO(&_readfds);
	std::vector<Server *>::iterator i = servers.begin();
	for(; i != servers.end(); i++)
	{
		(*i)->disconnect();
		delete (*i);
	}
	return 0;
}

// int main(int argc, char *argv[])
// {
// 	if (argc > 2)
// 	{
// 		std::cerr << RED << "Error: Wrong numbers of arguments" << RESET << std::endl;
// 		return 1;
// 	}
// 	// FD_ZERO(&_readfds);
// 	ConfigFile cf(argc == 1 ? "default_config_file.conf" : argv[1]);
// 	signal(SIGINT, signal_handler);
// 	execAutoindex();

// 	std::vector<Server *> servers = initServers(cf.GetMapConfig());
// 	Clients clients;

// 	int maxfd = -1;
// 	while (run)
// 	{
// 		std::string bufferStr;
// 		FD_ZERO(&_readfds);
// 		for (size_t i = 0; i < servers.size(); ++i)
// 		{
// 			int sockfd = servers[i]->GetSocketfd();
// 			FD_SET(sockfd, &_readfds);
// 			if (sockfd > maxfd)
// 				maxfd = sockfd;
// 		}
// 		// for (size_t i = 0; i < servers.size(); ++i)
// 		int num_ready = select(maxfd + 1, &_readfds, NULL, NULL, NULL);

// 		if (num_ready > 0)
// 		{
// 			for (size_t i = 0; i < servers.size(); ++i)
//         	{
// 				int sockfd = servers[i]->GetSocketfd();
// 				std::cout << "Sockfd at " << i << " dio porco "<< sockfd << std::endl;
// 				if (FD_ISSET(sockfd, &_readfds))
// 				{
// 						std::cout << "ho finito le idee"<< std::endl;
// 						ssize_t addrlen = sizeof(sockaddr);
// 						int connection = accept(sockfd, (struct sockaddr *)(*servers[i]).GetSocketAddr(), (socklen_t *)&addrlen);
// 						std::cout<<"accept return"<<connection<<std::endl;
// 						// for(size_t o = 0; o < servers.size(); o++)
// 						// 	std::cout << "sockfd & servers[i]->_socketfd "<< sockfd << " "<< servers[o]->GetSocketfd()<<std::endl;
// 						std::cout<<" 1 "<<std::endl;
// 						FD_SET(connection, &_readfds);
// 						printReadFDs(_readfds);

// 						if (connection < 0)
// 						{
// 							std::cerr << "Failed to accept connection. errno: " << errno << std::endl;
// 							continue;
// 						}
// 						else
// 						{
// 							servers[i]->SetServConn(connection);
// 							int index = findServerByFD(servers, connection);
// 							if (clients.conn_add(connection, sockfd))
// 							{
// 								ResponseHeader resHeader(NULL, std::make_pair("500", DEFAULT_ERROR_PATH));
// 								throw ServerException("500", resHeader.makeResponse(500), servers[i]->GetServConn());
// 							}
// 							else
// 							{
// 								// Leggi i dati inviati dal client
// 								char buffer[MAX_BUFFER_SIZE];
// 								int bytes_read = 0;
// 								int totalBytesRead = 0;
// 								do
// 								{
// 									bytes_read = recv(connection, buffer, 8192, 0);
// 									std::cout << "recv: " << bytes_read << std::endl;
// 									// bytesRead = recv(evList[i].ident, buffer, 8192, 0);
// 									totalBytesRead += bytes_read;
// 									if (bytes_read > 0)
// 										bufferStr.append(buffer, bytes_read);
// 									usleep(100);
// 								} while (bytes_read > 0);
// 								index = findServerByFD(servers, connection);
// 								Configuration config;
// 								RequestHeader reqHeader(bufferStr, bytes_read + 1);
// 								try
// 								{
// 									config = cf.GetConfig((*servers[index]).GetHostPort(), reqHeader.GetHost());
// 									if(config.isEmpty())
// 									{
// 										throw std::exception();
// 									}
// 								}
// 								catch(const std::out_of_range &e)
// 								{	
// 									continue;
// 								}
// 								catch(const std::exception &e)
// 								{
// 									std::cout<< "Error: no viable configuration found"<<std::endl;
// 									continue;
// 								}
// 								ResponseHeader resHeader = ResponseHeader(servers[index], &reqHeader, &config);
// 								std::cout << "la madonna con la cresta " << resHeader.getPath() << std::endl;
// 								std::string response;
// 								try{
// 									if(reqHeader.GetMethod() == "POST" && reqHeader.GetBody().length() > config.GetLimitSizeBody()){
// 										resHeader = ResponseHeader(NULL, std::make_pair("413", config.GetErrorPath("413")));
// 										throw ServerException(resHeader.getError().first,
// 											resHeader.makeResponse(std::atoi(resHeader.getError().first.c_str())), connection);
// 									}
// 									else {
// 										if(!config.GetRedirectionCode())
//                                         {
// 											response.append(resHeader.makeResponse(200));
//                                         }
//                                         else{
//                                             std::cout<<"Make response called\n";
// 											response.append(resHeader.makeResponse(config.GetRedirectionCode()));
// 											std::string redir("Location");
// 											response.append(config.GetRedirectionUrl());
// 											response.append("\r\n");
// 											response.insert(response.find('\n') + 1, redir);
// 										}
// 										if(!resHeader.getError().first.empty()){
// 											resHeader = ResponseHeader(NULL, resHeader.getError());
// 											throw ServerException(resHeader.getError().first,
// 												resHeader.makeResponse(std::atoi(resHeader.getError().first.c_str())), connection);
// 										}
// 									}
// 								}
// 								catch (const ServerException &e){
// 									response = e.what();
// 									std::cout<<"checking here"<<std::endl;
// 								}
// 									// servers[i]->closing();
// 								std::string chunck;
// 								int data_sent = 0;
								
// 								do{
// 									chunck = response.substr(0, 35000);
// 									data_sent = send(connection, chunck.c_str(), chunck.size(), 0);
// 									if(data_sent < 0)
// 										break;
// 									response.substr(data_sent);
// 								} while(response.size());
// 								bufferStr.clear();
// 								FD_CLR(connection, &_readfds);
// 								std::cout<< " 2 "<<std::endl;
// 								printReadFDs(_readfds);
// 								index = findServerByFD(servers, connection);
// 								close(connection);
// 								clients.conn_delete(connection);
// 								// servers[index]->SetServConn(0);
// 								usleep(1000);
// 								// i = findServerByFD(servers, connection);
// 								// FD_SET(sockfd, &_readfds);
// 								// std::cout<<"RETURN FROM CON_DELETE: "<<clients.conn_delete(servers[index]->GetSocketfd())<<std::endl;
// 								// clients.conn_delete(clients.Get_conn(servers[index]->GetSocketfd())->fd);
// 								//usleep(100);
// 								// if(reqHeader.GetMethod() == "GET")
// 								// {
// 								// 	response = resHeader.makeResponse(200);
// 								// }
// 								// else if (reqHeader.GetMethod() == "POST")
// 								// {
// 								// 	response = resHeader.makeResponse(413);
// 								// }
// 								// else
// 								// 	response = resHeader.makeResponse(405);
// 								// ssize_t byte_sent = send(connection, response.c_str(), response.length(), 0);
// 								// if (byte_sent < 0)
// 								// 	std::cerr << "madonna laida"<<std::endl;
// 							}
// 						}
// 						std::cout << "i'm in "<< std::endl;					
// 						}
// 				}
			
// 		}
// 		// else if (num_ready == 0)
// 		// {
// 		// 	continue;
// 		// }
// 		// else
// 		// {
// 		// 	break;
// 		// }
// 		std::cout << "aaaaaaaaaaaaaaa" << std::endl;
// 	}
// 		std::cout << RED << "EXIT" << RESET << std::endl;
// 		for (size_t i = 0; i < servers.size(); ++i)
// 		{
// 			servers[i]->disconnect();
// 			delete servers[i];
// 		}
// 		servers.clear();

// 		return 0;
// }
