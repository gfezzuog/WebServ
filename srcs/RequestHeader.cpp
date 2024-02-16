#include "../incl/WebServer.h"

RequestHeader::RequestHeader(std::string string, size_t n)
{
    parser(string, n);
}

RequestHeader::RequestHeader()
{}

RequestHeader::~RequestHeader()
{}

void RequestHeader::parser(const std::string string, size_t n) {
	std::istringstream resp(string);
	std::string header;
	std::string::size_type index;
	size_t j = 2;
	while (std::getline(resp, header) && header != "\r") {
		size_t pos = 0;
		while ((pos = header.find(13)) != std::string::npos)
			header.erase(pos);
		index = header.find(':', 0);
		if(index != std::string::npos)
		{
			std::pair<std::string, std::string> tmp = std::make_pair(header.substr(0, index), header.substr(index + 2));
			if (_mapRH.find("Cookie") != _mapRH.end()
				&& tmp.first == "Cookie")
				_mapRH["Cookie"].append("; " + tmp.second);
			else
				_mapRH.insert(tmp);
		}
		else
			_mapRH.insert(std::make_pair("Method", header));
		j += header.size() + 2;
	}
	if (n > j)
		_mapRH.insert(std::make_pair("Body", string.substr(j, n - j)));
}

std::map<std::string, std::string> RequestHeader::GetHeaders() const
{
    return _mapRH;
}


std::string RequestHeader::GetMethod() const
{
    try{
		return _mapRH.at("Method").substr(0, _mapRH.at("Method").find(' ', 0));
	}
	catch(const std::exception& e){	
		return std::string ();
	}
}

std::string RequestHeader::GetAccept() const
{
    return _mapRH.at("Accept");
}

std::string RequestHeader::GetHost() const
{
    return _mapRH.at("Host");
}

std::string RequestHeader::GetPath() const {
	size_t temp = _mapRH.at("Method").find(' ', 0) + 1;
	std::string fullPath = _mapRH.at("Method").substr(temp, _mapRH.at("Method").find(' ', temp + 1) - temp);
	temp = fullPath.find('?', 0);
	if (temp == std::string::npos)
		return fullPath;
	else
		return fullPath.substr(0, temp);
}


std::string RequestHeader::GetBody() const
{
    try
    {
        return _mapRH.at("Body");
    }
    catch(const std::exception& e)
    {
        return std::string();
    }
}

std::string RequestHeader::GetQueryString() const
{
    std::string path = _mapRH.at("Method").substr(_mapRH.at("Method").find(' ', 0) + 1);
    path = path.substr(0, path.find(' ', 0));
    std::string::size_type index = path.find('?', 0);
    if(index != std::string::npos)
        return path.substr(index + 1);
    return std::string();
}
