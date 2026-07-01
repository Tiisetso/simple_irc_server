#pragma once

#include <string>

class Server
{
private:
	int					_servSockFd;
	int					_clientFd;
	std::string			_port;
	std::string			_password;

public:
	Server() = delete;
	Server(const std::string& port, const std::string& password);
	Server(const Server& other) = delete;
	Server& operator=(const Server& other) = delete;
	~Server();

	void createSocket();
	void acceptClient();
};