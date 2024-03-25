#include "Parser.hpp"
#include "Request.hpp"

void removeInactiveClients(std::vector<std::pair<int, std::string> > cSockets, fd_set &act)
{
	std::vector<int> inactive;
	for (int i = 0; i < cSockets.size(); i++)
	{
		if (!FD_ISSET(cSockets[i].first, &act))
			inactive.push_back(cSockets[i].first);
	}
	for (int i = 0; i < inactive.size(); i++)
	{
		for (int j = 0; j < cSockets.size(); j++)
		{
			if (cSockets[i].first == inactive[j])
			{
				cSockets.erase(cSockets.begin() + j);
				break;
			}
		}
	}
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Wrong number of arguments, expecting only one config file" << std::endl;
		return 1;
	}
	if ((std::string(argv[1]).substr(std::string(argv[1]).find_last_of(".") + 1) != "conf"))
	{
		std::cout << "Invalid config file, exiting" << std::endl;
		return 1;
	}
	Parser Parsy = Parser(argv[1]);

	const Server *server;
	while ((server = Parsy.getServer()) != NULL)
	{
		std::cout << *server << std::endl;
	}

	std::vector<Server> Servers;
	Servers = Parsy.getServerVector();
	fd_set readfd, write, active;
	FD_ZERO(&active);
	for (size_t i = 0; i < Servers.size(); i++)
	{
		FD_SET(Servers[i].getSocket(), &active);
	}
	while (1)
	{
		readfd = write = active;

		if (select(FD_SETSIZE, &readfd, &write, NULL, NULL) < 0)
		{
			std::cerr << "Error: select: Bad file descriptor" << std::endl;
			break;
		}

		for (size_t i = 0; i < Servers.size(); i++)
		{
			if (FD_ISSET(Servers[i].getSocket(), &readfd))
			{
				int client_socket = accept(Servers[i].getSocket(), NULL, NULL); // Accept new connection
				if (client_socket < 0)
				{
					std::cerr << "Error: accept: Bad file descriptor | Interrupted system call | Too many open files" << std::endl;
					exit(1);
				}
				Servers[i].cSockets.push_back(std::make_pair(client_socket, ""));
				FD_SET(client_socket, &active);
			
			}
			for (int j = 0; j < Servers[i].cSockets.size(); j++)
			{
				int flag = 0;
				char bufferRead[65536];
				bzero(bufferRead, 65536);
				int res;
				if (FD_ISSET(Servers[i].cSockets[j].first, &readfd))
				{
					std::cout << Servers[i].cSockets[j].first << " pippo " << j << std::endl;
					int res = read(Servers[i].cSockets[j].first, bufferRead, 1025);
					if (res <= 0)
					{ // TODO se va male devo anche rimuovere il client no?
						flag = 1;
						Servers[i].cSockets[j].second = "";
					}
					else
					{

						bufferRead[res] = 0;
						Request Req = Request(bufferRead, Servers[i]);

						std::string response;

						response += "HTTP/1.1 ";
						response += Req.getStatus();
						response += "\r\n";
						// Headers
						response += "Content-Length: ";
						response += std::to_string(Req.getResponseBody().length());
						response += "\r\n";
						response += "Content-Type: text/html\r\n";
						response += "\r\n";
						response += Req.getResponseBody();

						Servers[i].cSockets[j].second = response;
						std::cout << response << std::endl;

						if (Req.getConnectionType().compare("keep-alive"))
							flag = 1;
					}
				}
				else if (FD_ISSET(Servers[i].cSockets[j].first, &write))
				{
					send(Servers[i].cSockets[j].first, Servers[i].cSockets[j].second.c_str(), Servers[i].cSockets[j].second.length(), 0); // TODO To check the returned value
					Servers[i].cSockets[j].second = "";
				}
				if ((flag && Servers[i].cSockets[j].second == ""))
				{
					FD_CLR(Servers[i].cSockets[j].first, &active);
					close(Servers[i].cSockets[j].first);
				}
			}
			removeInactiveClients(Servers[i].cSockets, active);
		}
	}

	while ((server = Parsy.getServer()) != NULL) // Clean up server sockets
	{
		close((*server).getSocket());
	}

	return 0;
}