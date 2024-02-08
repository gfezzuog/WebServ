#include "../incl/WebServer.h"

Clients::Clients()
{}

Clients::~Clients()
{}

//la funzione cerca un oggetto c_data nel vettore _clients in base al valore di fd. Se lo trova, restituisce un puntatore a quell'oggetto; altrimenti, restituisce NULL.
Clients::c_data *Clients::get_conn(int fd)
{
	int j = 0;
	std::vector<c_data>::iterator i = _clients.begin();
	for (; i != _clients.end(); i++, j++)
		if((*i).fd == fd)
			return(&(*i));
	return (NULL);
}

//La funzione fcntl, serve per controllare le proprietà di un fd, il primo argomento è fd, il secondo il comando e il terzo un argomento opzionale dipendete dal comando.
//int flags = fcntl(fd, F_GETFL, 0); qui controlliamo le flag del nostro fd, nel momento in cui assert non fallisce, settiamo le flag impostando la modalitá non bloccante: fcntl(fd, F_SETFL, flags | O_NONBLOCK);
int Clients::conn_add(int fd)
{
	if (fd < 1)
		return (-1);
	int flags = fcntl(fd, F_GETFL, 0);
	assert(flags >= 0); //è una funzione che controlla la veridicitá dell'espressione, se falsa stampa un messaggio di errore
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	_clients.push_back(c_data(fd));
	return(0);
}

//questa funzione serve per eliminare una connesione ad un fd fornito come argomento
int Clients::conn_delete(int fd)
{
	if (fd < 1)
		return(-1);
	c_data *cd = get_conn(fd);
	if (!cd)
		return(-1);
	std::vector<c_data>::iterator i =_clients.begin();
	for (; (*i).fd != (*cd).fd; i++)
		;
	_clients.erase(i);
	return close(fd);
}