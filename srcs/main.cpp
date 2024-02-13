#include "../incl/WebServer.h"

bool run = 1;

static void signal_handler(int i)
{
    if (i == SIGINT)
        run = 0;
}

// static int findServerByFD(std::vector<Server *> servers, int ident)
// {
//     int i = 0;
//     std::vector<Server *>::iterator it = servers.begin();
//     for (; it != servers.end(); it++, i++)
//         if ((**it).GetSocketfd() == ident)
//             return i;
//     return -1;
// }

std::vector<Server *> initServers(std::map<std::string, std::vector<Configuration> > portConfigs)
{
    std::map<std::string, std::vector<Configuration> >::iterator it = portConfigs.begin();
    std::vector<Server *> servers;

    for (; it != portConfigs.end(); it++)
    {
        Server *s = new Server((*it).second[0].GetPort(), (*it).second[0].GetHost(), &((*it).second[0]));
        try
        {
            s->initialize(); // Inizializza _readfds per questa istanza di Server
            s->connect();    // Connessione e aggiunta del socket a _readfds
        }
        catch (const OpenSocketException &e)
        {
            std::cout << RED << e.what() << RESET << std::endl;
        }
        servers.push_back(s);
    }
    return servers;
}
int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        std::cerr << RED << "Error: Wrong numbers of arguments" << RESET << std::endl;
        return 1;
    }

    ConfigFile cf(argc == 1 ? "default_config_file.conf" : argv[1]);
    signal(SIGINT, signal_handler);
    execAutoindex();

    std::vector<Server *> servers = initServers(cf.GetMapConfig());

    Clients clients;

    while (run)
    {
        fd_set readfds = {}; // Inizializza il set dei descrittori di file per la lettura
        int maxfd = -1;

        // Aggiunge tutti i socket al set readfds e tiene traccia del massimo fd
        for (size_t i = 0; i < servers.size(); ++i)
        {
            int sockfd = servers[i]->GetSocketfd();
            if (sockfd > maxfd)
                maxfd = sockfd;
        }

        // Copia _readfds di ogni server in readfds
        for (size_t i = 0; i < servers.size(); ++i)
        {
            FD_SET(servers[i]->GetSocketfd(), &servers[i]->_readfds);
        }

        // Attende gli eventi di lettura sui socket utilizzando select
        int num_ready = select(maxfd + 1, &readfds, NULL, NULL, NULL);
        if (num_ready > 0)
        {
            // Gestisce gli eventi pronti sui socket
            for (size_t i = 0; i < servers.size(); ++i)
            {
                int sockfd = servers[i]->GetSocketfd(); // Ottieni il descrittore di file del socket

                // Verifica se il socket è pronto per la lettura
                if (FD_ISSET(sockfd, &servers[i]->_readfds))
                {
                    // Leggi dati dal socket
                    char buffer[MAX_BUFFER_SIZE];
                    ssize_t bytes_read = recv(sockfd, buffer, MAX_BUFFER_SIZE, 0);
                    if (bytes_read < 0)
                    {
                        // Gestione degli errori durante la lettura dal socket
                        // Ad esempio, potresti chiudere il socket e rimuoverlo da _readfds
                        servers[i]->disconnect();
                        continue;
                    }
                    else if (bytes_read == 0)
                    {
                        // Connessione chiusa dal client
                        // Puoi chiudere il socket e rimuoverlo da _readfds
                        servers[i]->disconnect();
                        continue;
                    }

                    // Creazione dell'oggetto RequestHeader utilizzando i dati ricevuti dal socket
                    RequestHeader reqHeader(buffer, bytes_read);
                    // Creazione dell'oggetto ResponseHeader utilizzando RequestHeader e il server corrente
                    ResponseHeader resHeader(servers[i], &reqHeader, servers[i]->GetConfig()); // Dove configurationPointer è un puntatore alla configurazione del server

                    // Generazione della risposta
                    std::string response = resHeader.makeResponse(200); // Esempio: codice di risposta 200

                    // Invia la risposta al client
                    ssize_t bytes_sent = send(sockfd, response.c_str(), response.length(), 0);
                    if (bytes_sent < 0)
                    {
                        // Gestione degli errori durante l'invio della risposta al client
                        // Ad esempio, potresti chiudere il socket e rimuoverlo da _readfds
                        servers[i]->disconnect();
                    }
                }
                else if (num_ready == 0)
                {
                    // Nessun evento pronto, continua l'attesa
                    continue;
                }
                else
                {
                    // Errore nella chiamata a select, gestisci l'errore
                    std::cerr << "Error in select" << std::endl;
                    break;
                }
            }
        }
        std::cout << RED << "EXIT" << RESET << std::endl;
        // Chiudi i socket e dealloca le risorse
        for (size_t i = 0; i < servers.size(); ++i)
        {
            servers[i]->disconnect();
            delete servers[i];
        }
        servers.clear();

        return 0;
    }
}    // int main(int argc, char *argv[])
    // {
    // 	if (argc > 2)
    // 	{
    // 		std::cerr << RED << "Error: Wrong numbers of arguments" << RESET << std::endl;
    // 		return 1;
    // 	}
    // 	ConfigFile cf(argv[1]);
    // 	signal(SIGINT, signal_handler);
    // 	execAutoindex();

    // 	std::vector<Server *> servers = initServers(cf.GetMapConfig());

    // 	Clients clients;

    // 	// kevent evList[MAX_EVENTS];

    // 	while (run)
    // 	{
    // 		std::string bufferStr;
    // 		// num_events = kevent(kQueue, NULL, 0, evList, MAX_EVENTS, NULL);
    // 		//std::vector<Server *>::iterator it = servers.begin();
    // 		for (int i = 0; i < servers[i]->Getmaxfd(); i++)
    // 		{
    // 			int index = findServerByFD(servers, servers[i]->GetSocketfd());
    // 			if (index != -1)
    // 			{
    // 				ssize_t addrlen = sizeof(sockaddr);
    // 				int connection = accept(servers[i]->GetSocketfd(), (struct sockaddr *)(*servers[index]).GetSocketAddr(), (socklen_t *)&addrlen);
    // 				try
    // 				{
    // 					if (clients.conn_add(connection))
    // 					{
    // 						ResponseHeader resHeader(NULL, std::make_pair("500", DEFAULT_ERROR_PATH));
    // 						throw ServerException("500", resHeader.makeResponse(500), connection);
    // 					}
    // 					else
    // 					{
    // 						select(servers[i]->GetSocketfd(), servers[i]->GetFDS(), NULL, NULL, NULL);
    // 						// EV_SET((*servers[index]).GetEvSet(), connection, EVFILT_READ, EV_ADD, 0, 0, NULL);
    // 						// kevent(kQueue, (*servers[index]).GetEvSet(), 1, NULL, 0, NULL);
    // 					}
    // 				}
    // 				catch (const ServerException &e)
    // 				{
    // 					e.what();
    // 				}
    // 			}
    // 			else //  if (evList[i].filter == EVFILT_READ)
    // 			{
    // 				Configuration config;
    // 				char buffer[8192];
    // 				index = findServerByFD(servers, servers[i]->GetSocketfd());
    // 				size_t totalBytesRead = 0;
    // 				int bytesRead = 0;
    // 				do
    // 				{
    // 					bytesRead = recv(servers[i]->GetSocketfd(), buffer, 8192, 0);
    // 					totalBytesRead += bytesRead;
    // 					if (bytesRead > 0)
    // 						bufferStr.append(buffer, bytesRead);
    // 					usleep(100);
    // 				} while (bytesRead > 0);
    // 				// print header request
    // 				// for (size_t i = 0; i < totalBytesRead + 1; i++)
    // 				// 	std::cout << bufferStr[i];
    // 				RequestHeader reqHeader = RequestHeader(bufferStr, totalBytesRead + 1);
    // 				try
    // 				{
    // 					config = cf.GetConfig((*servers[index]).GetHostPort(), reqHeader.GetHost());
    // 					if (config.isEmpty())
    // 						throw std::exception();
    // 				}
    // 				catch (const std::out_of_range &e)
    // 				{
    // 					continue;
    // 				}
    // 				catch (const std::exception &e)
    // 				{
    // 					std::cout << RED << "Error: no vaiable configuration" << RESET << std::endl;
    // 					continue;
    // 				}
    // 				ResponseHeader resHeader = ResponseHeader(servers[index], &reqHeader, &config);
    // 				std::string resp;
    // 				try
    // 				{
    // 					if (reqHeader.GetMethod() == "POST" && reqHeader.GetBody().length() > config.GetLimitSizeBody())
    // 					{
    // 						resHeader = ResponseHeader(NULL, std::make_pair("413", config.GetErrorPath("413")));
    // 						throw ServerException(resHeader.GetError().first,
    // 							resHeader.makeResponse(std::atoi(resHeader.GetError().first.c_str())),
    // 							servers[i]->GetSocketfd());
    // 					}
    // 					else
    // 					{
    // 						if (!config.GetRedirectionCode())
    // 							resp.append(resHeader.makeResponse(200));
    // 						else
    // 						{
    // 							resp.append(resHeader.makeResponse(config.GetRedirectionCode()));
    // 							std::string redir("Location: ");
    // 							redir.append(config.GetRedirectionUrl());
    // 							redir.append("\r\n");
    // 							resp.insert(resp.find('\n') + 1, redir);
    // 						}
    // 						if (!resHeader.GetError().first.empty())
    // 						{
    // 							resHeader = ResponseHeader(NULL, resHeader.GetError());
    // 							throw ServerException(resHeader.GetError().first,
    // 								resHeader.makeResponse(std::atoi(resHeader.GetError().first.c_str())),
    // 								servers[i]->GetSocketfd());
    // 						}
    // 					}
    // 				}
    // 				catch (const ServerException &e)
    // 				{
    // 					resp = e.what();
    // 				}
    // 				std::string respChunck;
    // 				int dataSent = 0;
    // 				do
    // 				{
    // 					respChunck = resp.substr(0, 35000);
    // 					dataSent = send(servers[i]->GetSocketfd(), respChunck.c_str(), respChunck.size(), 0);
    // 					if (dataSent < 0)
    // 						break;
    // 					resp = resp.substr(dataSent);
    // 				} while (resp.size());
    // 				bufferStr.clear();
    // 				index = findServerByFD(servers, servers[i]->GetSocketfd());
    // 				select(servers[i]->GetSocketfd(), servers[i]->GetFDS(), NULL, NULL, NULL);
    // 				// EV_SET((*servers[index]).GetEvSet(), evList[i].ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    // 				// kevent(kQueue, (*servers[index]).GetEvSet(), 1, NULL, 0, NULL);
    // 				clients.conn_delete(servers[i]->GetSocketfd());
    // 				usleep(100);
    // 			}
    // 		}
    // 	}

    // 	std::cout << RED << "EXIT" << RESET << std::endl;
    // 	// close(kQueue);
    // 	std::vector<Server *>::iterator i = servers.begin();
    // 	for (; i != servers.end(); i++)
    // 	{
    // 		(*i)->disconnect();
    // 		delete (*i);
    // 	}
    // 	return 0;
    // }