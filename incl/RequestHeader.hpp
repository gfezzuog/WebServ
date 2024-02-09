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

		std::map<std::string, std::string> GetHeaders() const;
		std::string GetHost() const;
		std::string GetMethod() const;
		std::string GetBody() const;
		std::string GetPath() const;
		std::string GetQueryString() const;
		std::string GetAccept() const;
};