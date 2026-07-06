#include "Server.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>

#define BACKLOG 20

Server::Server(const std::string &port, const std::string &password)
	: _servSockFd(-1), _clientFd(-1), _port(port), _password(password)
{
}

Server::~Server()
{
	if (_servSockFd != -1)
		close(_servSockFd);
	if (_clientFd != -1)
		close(_clientFd);
}

int Server::acceptClient()
{
	// prepare client address struct for connect to fill in
	char clientAddrStr[INET6_ADDRSTRLEN];
	struct sockaddr_in clientAddr{};
	socklen_t clientAddrlen = sizeof(clientAddr);

	std::cout << "Server: Waiting for connection..." << std::endl;
	_clientFd = accept(_servSockFd, reinterpret_cast<sockaddr *>(&clientAddr),
					   &clientAddrlen);
	if (_clientFd == -1)
		throw std::runtime_error(
			std::string("Server: Failed to accept incoming connection: ") +
			std::strerror(errno));
	if (!inet_ntop(AF_INET, &(clientAddr.sin_addr), clientAddrStr,
				   INET6_ADDRSTRLEN))
		throw std::runtime_error(
			std::string("Server: Failed to convert client IP. ") +
			std::strerror(errno));
	std::cout << "Server: Connection accepted! Client fd: " << _clientFd << " "
			  << "Client address: " << clientAddrStr << std::endl;
	return _clientFd;
}

void Server::createSocket()
{
	// loading up data for holding server address information
	int ret;
	int socketyes = 0;
	struct addrinfo hints{};
	struct addrinfo *res = nullptr;
	struct addrinfo *temp = nullptr;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	ret = getaddrinfo(nullptr, _port.c_str(), &hints, &res);
	if (ret)
		throw std::runtime_error(std::string("Server: Failed getaddrinfo, ") +
								 gai_strerror(ret));

	// Create a socket and bind it to an address
	for (temp = res; temp != nullptr; temp = temp->ai_next)
	{
		_servSockFd =
			socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
		if (_servSockFd == -1)
			continue;
		socketyes = 1;
		std::cout << "Server: Socket created, fd = " << _servSockFd
				  << std::endl;

		int sockopt = 1;
		if (setsockopt(_servSockFd, SOL_SOCKET, SO_REUSEADDR, &sockopt,
					   sizeof(sockopt)) == -1)
			throw std::runtime_error(
				std::string("Server: Failed to set socket options: ") +
				std::strerror(errno));

		if (bind(_servSockFd, temp->ai_addr, temp->ai_addrlen) == 0)
			break;
		close(_servSockFd);
		_servSockFd = -1;
	}
	freeaddrinfo(res);

	if (socketyes == 0)
		throw std::runtime_error("Server: Failed creating socket");
	else if (temp == nullptr)
		throw std::runtime_error(std::string("Server: Failed to bind: ") +
								 std::strerror(errno));
	std::cout << "Server: Bind succeeded" << std::endl;

	// Start listening for incoming connections
	if (listen(_servSockFd, BACKLOG) == -1)
		throw std::runtime_error(std::string("Server: Failed to listen: ") +
								 std::strerror(errno));
	std::cout << "Server: Listening for incoming connections on port: " << _port
			  << std::endl;
}

void Server::processClient(User &client)
{
	const std::string greeting = "Moi Hej Hello 你好\r\n";
	char buffer[1024];
	ssize_t bytesReceived{};

	if (send(client.Fd, greeting.c_str(), greeting.length(), 0) < 0)
		throw std::runtime_error(std::string("Server: Failed to send: ") +
								 std::strerror(errno));

	bytesReceived = recv(client.Fd, buffer, sizeof(buffer) - 1, 0);
	if (bytesReceived < 0)
		throw std::runtime_error(std::string("Server: Failed to receive: ") +
								 std::strerror(errno));
	else if (bytesReceived == 0)
	{
		std::cout << "Server: Client disconnected" << std::endl;
		// Clear frees entire string if a d/c happens
		client.readBuffer.clear();
	}
	else
	{
		// GNL stuff
		buffer[bytesReceived] = '\0';
		client.readBuffer += buffer;

		size_t newlinePos = client.readBuffer.find('\n');

		while (newlinePos < client.readBuffer.size())
		{
			std::string fullMsg = client.readBuffer.substr(0, newlinePos + 1);
			// Erase can be pointed where on a str to free
			client.readBuffer.erase(0, newlinePos + 1);

			std::cout << "Server: Received: " << fullMsg.length()
					  << " bytes: " << fullMsg << std::endl;

			newlinePos = client.readBuffer.find('\n');
		}
	}
}