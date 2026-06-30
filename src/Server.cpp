#include "Server.hpp"
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdexcept>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

#define BACKLOG 20

Server::Server(const std::string& port):_servSockFd(-1), _port(port)
{
}

Server::~Server()
{
	if (_servSockFd != -1)
		close(_servSockFd);
}

void Server::runPollLoop()
{
	//prepare client address struct for connect to fill in
	struct sockaddr_in	client_addr{};
	socklen_t			client_addrlen = sizeof(client_addr);

	std::cout << "Waiting for connection..." << std::endl;
	int client_fd;
	client_fd = accept(_servSockFd, reinterpret_cast<sockaddr *>(&client_addr), &client_addrlen);
	if (client_fd == -1)
		throw std::runtime_error(std::string("Failed to accept incoming connection:") + std::strerror(errno));
	std::cout << "Connection accepted! " << std::endl;
	std::cout << "client fd: " << client_fd << std::endl;;
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
		throw std::runtime_error(std::string("Server: Failed getaddrinfo") + gai_strerror(ret));

	// Create a socket and bind it to an address
	for (temp = res; temp != nullptr; temp = temp->ai_next)
	{
		_servSockFd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
		if (_servSockFd == -1)
			continue;
		std::cout << "Server: socket created, fd = " << _servSockFd << std::endl;

		if (bind(_servSockFd, temp->ai_addr, temp->ai_addrlen) == 0)
			break;
		close(_servSockFd);
		_servSockFd = -1;
	}
	freeaddrinfo(res);

	if (_servSockFd == -1)
		throw std::runtime_error("Server: Failed creating socket");
	else if (temp == nullptr)
		throw std::runtime_error(std::string("Server: Failed to bind: ") + std::strerror(errno));
	std::cout << "Server: bind succeeded" << std::endl;

	// Start listening for incoming connections
	if (listen(_servSockFd, BACKLOG) == -1)
		throw std::runtime_error(std::string("Server: Failed to listen: ") + std::strerror(errno));
	std::cout << "Server: listening for incoming connections on port: " << _port << std::endl;
}
