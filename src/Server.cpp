#include "Server.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
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
	: _servSockFd(-1), _port(port), _password(password)
{
}

Server::~Server()
{
	if (_servSockFd != -1)
		close(_servSockFd);
}

// NOTE: Commented out code deliberately left behind as I modified it but don't
// know all the stuff going on. So leaving intentionally for comparison
// purposes.
//  int Server::acceptClient()
//  {
//  	// prepare client address struct for connect to fill in
//  	char clientAddrStr[INET6_ADDRSTRLEN];
//  	struct sockaddr_in clientAddr{};
//  	socklen_t clientAddrlen = sizeof(clientAddr);

// 	std::cout << "Server: Waiting for connection..." << std::endl;
// 	_clientFd = accept(_servSockFd, reinterpret_cast<sockaddr *>(&clientAddr),
// 					   &clientAddrlen);
// 	if (_clientFd == -1)
// 		throw std::runtime_error(
// 			std::string("Server: Failed to accept incoming connection: ") +
// 			std::strerror(errno));
// 	if (!inet_ntop(AF_INET, &(clientAddr.sin_addr), clientAddrStr,
// 				   INET6_ADDRSTRLEN))
// 		throw std::runtime_error(
// 			std::string("Server: Failed to convert client IP. ") +
// 			std::strerror(errno));
// 	std::cout << "Server: Connection accepted! Client fd: " << _clientFd << " "
// 			  << "Client address: " << clientAddrStr << std::endl;
// 	return _clientFd;
// }

void Server::acceptClients()
{
	while (true)
	{
		sockaddr_in clientAddr{};
		socklen_t clientAddrlen = sizeof(clientAddr);

		int clientfd =
			accept(_servSockFd, reinterpret_cast<sockaddr *>(&clientAddr),
				   &clientAddrlen);

		if (clientfd == -1)
		{
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return;

			if (errno == EINTR)
				continue;

			throw std::runtime_error(
				std::string("Server: Failed to accept incoming connection: ") +
				std::strerror(errno));
		};

		setNonBlocking(clientfd);

		_users[clientfd].setfd(clientfd);

		pollfd clientPollfd;
		clientPollfd.fd = clientfd;
		clientPollfd.events = POLLIN;
		clientPollfd.revents = 0;

		_pollfds.push_back(clientPollfd);
		std::cout << "Server: New client at fd: " << clientfd << std::endl;
	}
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

	setNonBlocking(_servSockFd);
	// Start listening for incoming connections
	if (listen(_servSockFd, BACKLOG) == -1)
		throw std::runtime_error(std::string("Server: Failed to listen: ") +
								 std::strerror(errno));
	std::cout << "Server: Listening for incoming connections on port: " << _port
			  << std::endl;

	pollfd serverPollfd;

	serverPollfd.fd = _servSockFd;
	serverPollfd.events = POLLIN;
	serverPollfd.revents = 0;

	_pollfds.push_back(serverPollfd);
}

int Server::processClient(User &client)
{
	// const std::string greeting = "Moi Hej Hello 你好\r\n";
	char buffer[1024];
	ssize_t bytesReceived{};

	// if (send(client.getFd(), greeting.c_str(), greeting.length(), 0) < 0)
	// 	throw std::runtime_error(std::string("Server: Failed to send: ") +
	// 							 std::strerror(errno));

	bytesReceived = recv(client.getfd(), buffer, sizeof(buffer) - 1, 0);

	if (bytesReceived < 0)
		throw std::runtime_error(std::string("Server: Failed to receive: ") +
								 std::strerror(errno));
	else if (bytesReceived == 0)
	{
		std::cout << "Server: Client disconnected" << std::endl;
		// Clear frees entire string if a d/c happens
		client.getReadBuffer().clear();
	}
	else
	{
		// GNL stuff
		buffer[bytesReceived] = '\0';
		client.getReadBuffer() += buffer;

		size_t newlinePos = client.getReadBuffer().find('\n');

		while (newlinePos < client.getReadBuffer().size())
		{
			std::string fullMsg =
				client.getReadBuffer().substr(0, newlinePos + 1);
			// Erase can be pointed where on a str to free
			client.getReadBuffer().erase(0, newlinePos + 1);

			std::cout << "Server: Received: " << fullMsg.length()
					  << " bytes: " << fullMsg << std::endl;

			newlinePos = client.getReadBuffer().find('\n');
		}
	}
}

void Server::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throw std::runtime_error(std::string("Server: fnctl flags failed: ") +
								 std::strerror(errno));
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throw std::runtime_error(std::string("Server: fnctl flags failed: ") +
								 std::strerror(errno));
}

void Server::removeClient(std::size_t i)
{
	int fd = _pollfds[i].fd;

	_pollfds[i] = _pollfds.back();
	_pollfds.pop_back();

	_users.erase(fd);

	std::cout << "Server: Removed client at fd: " << fd << std::endl;
}

void Server::loop()
{
	while (true)
	{
		int ready = poll(_pollfds.data(), _pollfds.size(), -1);
		
		if (ready == -1)
		{
			if(errno = -1)
				continue;
				
			throw std::runtime_error(std::string("Server: Poll failed: ") + std::strerror(errno));	
		}
		
		if(_pollfds[0].revents & POLLIN)
			acceptClients();
		
		for (std::size_t i = 1; i < _pollfds.size();)
        {
            int fd = _pollfds[i].fd;
            short events = _pollfds[i].revents;
            bool keepClient = true;

            if (events & POLLIN)
            {
                User &client = _users.at(fd);
                keepClient = processClient(client);
            }

            if (events & (POLLERR | POLLHUP | POLLNVAL))
                keepClient = false;

            if (!keepClient)
                removeClient(i);
            else
                ++i;
        }
	}


}
