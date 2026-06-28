#include "Server.hpp"
#include <iostream>
#include <netdb.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BACKLOG 20

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
		if (bind(_servSockFd, temp->ai_addr, temp->ai_addrlen) == 0)
			break;
	}
	if (_servSockFd == -1)
	{
		freeaddrinfo(res);
		throw std::runtime_error("Server: Failed creating socket");
	}
	else if (temp == nullptr)
	{
		freeaddrinfo(res);
		throw std::runtime_error("Server: Failed to bind");
	}
	std::cout << "bind " << std::endl;
	if (listen(_servSockFd, BACKLOG))
	{
		freeaddrinfo(res);
		throw std::runtime_error("Server: Failed to listen");
	}
	std::cout << "listening for incoming connection " << std::endl;
	freeaddrinfo(res);
}


