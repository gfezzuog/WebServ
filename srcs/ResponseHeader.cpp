# include "../incl/WebServer.h"

int execAutoindex()
{
	pid_t pid = fork();
	if (pid == 0)
	{
		std::string intrepreter = PYTHON3PATH;
		const char *pythonArgs[] = {PYTHON3PATH, AUTOINDEXPATH, NULL};
		char *envp[] = {NULL};
		execve(intrepreter.c_str(), const_cast< char ** >(pythonArgs), envp);
		exit(0);
	}
	return 0;
}

ResponseHeader::ResponseHeader(Server *server, RequestHeader *request, Configuration *config)
	: _server(server), _request(request), _config(config)
{
	setCodeMap();
	setPath();
	setContent();
	setContentType(_path);
}

ResponseHeader::ResponseHeader(RequestHeader *request, std::pair<std::string, std::string> error)
	: _request(request), _path(error.second), _error(error)
{
	setCodeMap();
	setContentType(_error.second);
	setContent();
}

ResponseHeader::~ResponseHeader()
{
}

std::string ResponseHeader::makeResponse(int code) const
{
	std::string response("HTTP/1.1 ");

	if (_contentLenght == 0)
	{
		response.append(to_string(204));
		response.append(" ");
		response.append(GetResponseCode(204));
	}
	else
	{
		response.append(to_string(code));
		response.append(" ");
		response.append(GetResponseCode(code));
	}
	response.append("\r\n");
	response.append("content-type: ");
	response.append(_contentType);
	response.append("\r\n");
	response.append("content-length: ");
	response.append(to_string(_contentLenght));
	response.append("\r\n");
	response.append(GetDate());
	response.append("\r\n");
	response.append(_content);
	return response;
}

void ResponseHeader::setCodeMap()
{
	_codeMap[200] = "OK";
	_codeMap[204] = "No Content";
	_codeMap[301] = "Moved Permanently";
	_codeMap[308] = "Permanent Redirect";
	_codeMap[400] = "Bad Request";
	_codeMap[403] = "Forbidden";
	_codeMap[404] = "Not Found";
	_codeMap[405] = "Method Not Allowed";
	_codeMap[406] = "Not Acceptable";
	_codeMap[413] = "Request Too Large";
	_codeMap[500] = "Internal Server Error";
}

void ResponseHeader::setPath()
{
	std::string path = _request->GetPath();
	ConfigurationRoute route = GetMatchingRoute(path);

	if (route.GetMethods().rfind(_request->GetMethod()) == std::string::npos)
		_error = std::make_pair("405", _config->GetErrorPath("405"));
	std::string::size_type temp = path.rfind(route.GetPath(), 0);
	if (temp == 0) temp = route.GetPath().length();
	if (!route.GetRoot().empty())
		_path = route.GetRoot().append("/" + path.substr(temp, path.size() - temp));
	else
		_path = path;
	ConfigurationRoute newRoute = GetMatchingRoute(_path);
	if (newRoute.GetMethods().rfind(_request->GetMethod()) == std::string::npos)
		_error = std::make_pair("405", _config->GetErrorPath("405"));
	if (_path[0] == '/')
		_path = _path.substr(1, _path.size() - 1);
	if (_path[_path.size() - 1] == '/')
		_path = _path.substr(0, _path.size() - 1);
	if (!newRoute.GetIndex().empty() && _path == newRoute.GetPath().substr(1, _path.size()))
		_path.append(newRoute.GetIndex());
	if (((newRoute.GetRoot().size() == 0 && newRoute.GetPath().size() > 0
		&& _path == newRoute.GetPath().substr(1, _path.size()))
		|| (route.GetRoot().size() > 0
		&& _path == route.GetRoot().substr(1, _path.size())))
		&& route.GetAutoIndex())
	{
		std::ifstream f(_path.c_str());
		struct stat s;
		stat(_path.c_str(), &s);
		if (f.fail() || s.st_mode & S_IFDIR)
		{
			execAutoindex();
			if (_path.size() == 0)
				_path = ".";
			_path.append("/.index.html");
		}
		else
			f.close();
	}
	// std::cout << _path << std::endl;
}

void ResponseHeader::setContent()
{
	std::ifstream file;
	file.open(_path.c_str(), std::ios::in | std::ios::binary);
	struct stat s;
	stat(_path.c_str(), &s);
	if (access(_path.c_str(), F_OK) == 0 && access(_path.c_str(), R_OK) != 0)
	{
		_error = std::make_pair("403", _config->GetErrorPath("403"));
		return ;
	}
	if (file.is_open() && !(s.st_mode & S_IFDIR))
	{
		size_t dotPos = _path.rfind(".");
		std::string type = dotPos != std::string::npos ? _path.substr(dotPos, _path.size() - dotPos) : "";
		if (_request && _request->GetMethod() == "DELETE")
		{
			if (remove(_path.c_str()) == 0)
				_content = "\r\n<h1>File deleted successfully</h1>";
			else
				_content = "\r\n<h1>Unable to delete the file</h1>";
			_contentLenght = _content.size() - 2;
		}
		else if (_error.first.empty() && (type == ".php" || type == ".py"))
		{
			ConfigurationRoute route = GetMatchingRoute(_request->GetPath());
			if (route.GetCGIPath()[0] == type)
			{
				setEnv();
				_content = executeCgi(route.GetCGIPath());
				_contentLenght = _content.size();
			}
			else
			{
					std::string tmp = std::string("\r\n");
				file.seekg(0, std::ios::end);
				_content.reserve(file.tellg());
				file.seekg(0, std::ios::beg);
				_content.assign((std::istreambuf_iterator<char>(file)),
					std::istreambuf_iterator<char>());
				_content = tmp.append(_content, 0, _content.size());
				_contentLenght = _content.size() - 2;
			}
		}
		else
		{
			std::string tmp = std::string("\r\n");
			file.seekg(0, std::ios::end);
			_content.reserve(file.tellg());
			file.seekg(0, std::ios::beg);
			_content.assign((std::istreambuf_iterator<char>(file)),
				std::istreambuf_iterator<char>());
			_content = tmp.append(_content, 0, _content.size());
			_contentLenght = _content.size() - 2;
		}
	}
	else
		_error = std::make_pair("404", _config->GetErrorPath("404"));
	file.close();
}

void ResponseHeader::setContentType(std::string path, std::string type)
{
	ConfigurationRoute route;
	if (_request)
		route = GetMatchingRoute(_request->GetPath());

	if (type != "")
	{
		_contentType = type;
		return ;
	}
	size_t dotPos = path.rfind(".");
	type = dotPos != std::string::npos ? path.substr(dotPos + 1, path.size() - dotPos) : "";
	if (type == "html" || (type == "php" && route.GetCGIPath()[0] == "php")
		|| (type == "py" && route.GetCGIPath()[0] == "py") || _request->GetMethod() == "DELETE")
		_contentType = "text/html";
	else if (type == "css")
		_contentType = "text/css";
	else if (type == "js")
		_contentType = "text/javascript";
	else if (type == "jpeg" || type == "jpg")
		_contentType = "image/jpeg";
	else if (type == "png")
		_contentType = "image/png";
	else if (type == "bmp")
		_contentType = "image/bmp";
	else
		_contentType = "text/plain";
	// if (GetError().first.empty() && _request->GetAccept().rfind(_contentType, 0) == std::string::npos)
	// 		_error = std::make_pair("406", _config.GetErrorPath("406"));
}

std::string ResponseHeader::GetResponseCode(int code) const
{
	return _codeMap.at(code);
}

std::string ResponseHeader::GetDate() const
{
	std::string date = "date: ";
	time_t t = std::time(NULL);
	tm* lt = std::localtime(&t);
	char buffer[50];
	std::strftime(buffer, 80, "%a, %d %b %Y %X", lt);
	date.append(std::string(buffer));
	return date;
}

ConfigurationRoute ResponseHeader::GetMatchingRoute(std::string path) const
{
	std::map<std::string, ConfigurationRoute> configRoute = _config->GetConfigsRoute();
	std::map<std::string, ConfigurationRoute>::reverse_iterator it = configRoute.rbegin();
	while (it != configRoute.rend()) {
		if (path.rfind(it->first, 0) == 0)
			return it->second;
		it++;
	}
	std::map<std::string, ConfigurationRoute> tmp = _server->GetConfig().GetConfigsRoute();
	if (tmp.find(std::string("/")) == tmp.end())
	{
		std::cerr << RED << "Error: Parsing configuration file is gone wrong" << RESET << std::endl;
		exit(1);
	}
	return tmp.find(std::string("/"))->second;
}

std::pair<std::string, std::string> ResponseHeader::GetError() const
{
	return _error;
}

void ResponseHeader::setEnv() {
	std::map<std::string, std::string>	headers = _request->GetHeaders();
	char cwd[9999];
	getcwd(cwd, sizeof(cwd));
	if (headers.find("Auth-Scheme") != headers.end() && headers["Auth-Scheme"] != "")
		this->_env["AUTH_TYPE"] = headers["Authorization"];

	this->_env["REDIRECT_STATUS"] = "200";
	this->_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_env["SCRIPT_NAME"] = _path;
	this->_env["SCRIPT_FILENAME"] = _path;
	this->_env["REQUEST_METHOD"] = _request->GetMethod();
	this->_env["CONTENT_LENGTH"] = to_string(_request->GetBody().length());
	this->_env["CONTENT_TYPE"] = headers["Content-Type"];
	this->_env["PATH_INFO"] = _path;
	this->_env["PATH_TRANSLATED"] =_path;
	this->_env["QUERY_STRING"] = _request->GetQueryString();
	this->_env["REMOTEaddr"] = _config->GetHost();
	this->_env["REMOTE_IDENT"] = headers["Authorization"];
	this->_env["REMOTE_USER"] = headers["Authorization"];
	this->_env["REQUEST_URI"] = _path + _request->GetQueryString();
	this->_env["UPLOAD_PATH"] = std::string(cwd) + GetMatchingRoute(_request->GetPath()).GetUploadPath();
	if (headers.find("Hostname") != headers.end())
		this->_env["SERVER_NAME"] = headers["Hostname"];
	else
		this->_env["SERVER_NAME"] = this->_env["REMOTEaddr"];
	this->_env["SERVER_PORT"] = to_string(_config->GetPort());
	this->_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	this->_env["SERVER_SOFTWARE"] = "Webserv/1.0";
	this->_env["HTTP_COOKIE"] = headers["Cookie"];
}

char **ResponseHeader::GetEnvAsCstrArray() const {
	char	**env = new char*[this->_env.size() + 1];
	int	j = 0;
	for (std::map<std::string, std::string>::const_iterator i = this->_env.begin(); i != this->_env.end(); i++) {
		std::string	element = i->first + "=" + i->second;
		env[j] = new char[element.size() + 1];
		env[j] = strcpy(env[j], (const char*)element.c_str());
		j++;
	}
	env[j] = NULL;
	return env;
}

std::string ResponseHeader::executeCgi(const std::vector<std::string>& cgiPar) {
	pid_t		pid;
	int			saveStdin;
	int			saveStdout;
	char		**env;
	std::string	newBody;

	try {
		env = this->GetEnvAsCstrArray();
	}
	catch (std::bad_alloc &e) {
		std::cerr << RED << e.what() << RESET << std::endl;
	}

	saveStdin = dup(STDIN_FILENO);
	saveStdout = dup(STDOUT_FILENO);

	FILE* fileIn = tmpfile();
	FILE* fileOut = tmpfile();
	int fdIn = fileno(fileIn);
	int fdOut = fileno(fileOut);

	int		ret = 1;
	write(fdIn, _request->GetBody().c_str(), _request->GetBody().size());
	lseek(fdIn, 0, SEEK_SET);

	pid = fork();

	if (pid == -1)
	{
		std::cerr << RED << "Fork crashed." << RESET << std::endl;
		_error = std::make_pair("500", DEFAULT_ERROR_PATH);
		return ("");
	}
	else if (!pid)
	{
		char * const * nll = NULL;

		dup2(fdIn, STDIN_FILENO);
		dup2(fdOut, STDOUT_FILENO);
		const char *pythonArgs[] = {cgiPar[1].c_str(), _path.c_str(), NULL};
		if (cgiPar[0] == ".py")
			execve(cgiPar[1].c_str(), const_cast< char ** >(pythonArgs), env);
		else
			execve(cgiPar[1].c_str(), nll, env);
		std::cerr << RED << "Execve crashed." << RESET << std::endl;
		_error = std::make_pair("500", DEFAULT_ERROR_PATH);
	}
	else
	{
		char	buffer[65536] = {0};
		while (waitpid(-1, NULL, 2) != -1) ;
		lseek(fdOut, 0, SEEK_SET);
		while (ret > 0)
		{
			memset(buffer, 0, 65536);
			ret = read(fdOut, buffer, 65536 - 1);
			newBody += buffer;
		}
	}

	dup2(saveStdin, STDIN_FILENO);
	dup2(saveStdout, STDOUT_FILENO);
	fclose(fileIn);
	fclose(fileOut);
	close(fdIn);
	close(fdOut);
	close(saveStdin);
	close(saveStdout);
	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;

	if (!pid)
		exit(0);

	return (newBody);
}