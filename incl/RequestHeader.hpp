#pragma once

#include "WebServer.h"

class RequestHeader
{
	private:
		std::map<std::string, std::string> _mapRH;
		void parser(const std::string string, size_t n);

	public:
		RequestHeader();
		RequestHeader(const std::string string, size_t n);
		~RequestHeader();

		std::map<std::string, std::string> getHeaders() const;
		std::string getHost() const;
		std::string getMethod() const;
		std::string getBody() const;
		std::string getPath() const;
		std::string getQuaryString() const;
		std::string getAccept() const;
};