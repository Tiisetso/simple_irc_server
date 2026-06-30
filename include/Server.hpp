#pragma once

#include <string>

class Server
{
private:
	int					_servSockFd;
	std::string			_port;

public:
	Server() = delete;
	Server(const std::string& port);
	Server(const Server& other) = delete;
	Server& operator=(const Server& other) = delete;
	~Server();

	void createSocket();
	void runPollLoop();
};