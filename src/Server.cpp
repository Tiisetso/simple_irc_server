#include "Server.hpp"
#include <iostream>
#include <netdb.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

Server::Server(const std::string& port):_servSockFd(-1), _port(port)
{
}

Server::~Server()
{
	if (_servSockFd != -1)
		close(_servSockFd);
}

void Server::createSocket()
{
	//loading up data for holding server address information
	int					ret;
	struct addrinfo		hints{};
	struct addrinfo*	res = nullptr;
	struct addrinfo*	temp = nullptr;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	ret = getaddrinfo(nullptr, _port.c_str(), &hints, &res);
	if (ret)
		throw std::runtime_error("Server: Failed getaddrinfo");

	for (temp = res; temp != nullptr; temp = temp->ai_next)
	{
		_servSockFd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
		if (_servSockFd == -1)
			continue;
		std::cout << "server socket created, socket fd: " << _servSockFd << std::endl;
		//socket is created, now we can use the right address to bind
		break;
	}
	if (_servSockFd == -1)
	{
		freeaddrinfo(res);
		throw std::runtime_error("Server: Failed creating socket");
	}

	freeaddrinfo(res);
}


