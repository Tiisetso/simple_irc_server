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
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include "User.hpp"
#include "Utilities.hpp"
#include "Channel.hpp"

#define BACKLOG 20
#define MAX_MSG_LEN 512
#define MAX_WRITE_BUFFER (200 * 1024)

static std::string getCurrentDate()
{
	std::time_t now = std::time(NULL);
	if (now == static_cast<std::time_t>(-1))
		return "1970-01-01";

	std::tm *timeinfo = std::localtime(&now);

	char buffer[11];
	if (timeinfo &&
		std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo) > 0)
		return buffer;

	return "1970-01-01";
}

Server::Server(const std::string &port, const std::string &password)
	: _servSockFd(-1),
	  _port(port),
	  _password(password),
	  _createdAt(getCurrentDate())
{
}

Server::~Server()
{
	if (_servSockFd != -1)
		close(_servSockFd);
}

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
		}

		char clientAddrStr[INET_ADDRSTRLEN] = {};

		if (!inet_ntop(AF_INET, &clientAddr.sin_addr, clientAddrStr,
					   sizeof(clientAddrStr)))
		{
			close(clientfd);
			throw std::runtime_error("Server: Failed to convert client IP");
		}

		try
		{
			_users[clientfd].setFd(clientfd);
			_users[clientfd].setHost(clientAddrStr);
		}
		catch (...)
		{
			close(clientfd);
			throw;
		}

		try
		{
			setNonBlocking(clientfd);
			pollfd clientPollfd;
			clientPollfd.fd = clientfd;
			clientPollfd.events = POLLIN;
			clientPollfd.revents = 0;
			_pollfds.push_back(clientPollfd);
		}
		catch (...)
		{
			_users.erase(clientfd);
			throw;
		}
		std::cout << "Server: New client at fd: " << clientfd
				  << ", address: " << clientAddrStr << std::endl;
	}
}

void Server::createSocket()
{
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
		{
			std::string err = std::strerror(errno);
			freeaddrinfo(res);
			throw std::runtime_error(
				std::string("Server: Failed to set socket options: ") + err);
		}
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

bool Server::readClient(User &client)
{
	char buffer[1024];
	ssize_t bytesReceived{};

	bytesReceived = recv(client.getFd(), buffer, sizeof(buffer), 0);

	if (bytesReceived < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			return true;

		std::cerr << "Server: Failed to receive: " << client.getFd() << " : "
				  << std::strerror(errno) << std::endl;
		return false;
	}
	else if (bytesReceived == 0)
	{
		std::cout << "Server: Client disconnected" << std::endl;
		return false;
	}
	else
	{
		client.getReadBuffer().append(buffer, bytesReceived);

		std::size_t newlinePos = client.getReadBuffer().find('\n');

		while (newlinePos != std::string::npos)
		{
			if (newlinePos >= MAX_MSG_LEN)
			{
				std::cerr << "Server: Message too long (" << MAX_MSG_LEN
						  << " bytes maximum)." << std::endl;
				return false;
			}

			std::string fullMsg =
				client.getReadBuffer().substr(0, newlinePos + 1);

			client.getReadBuffer().erase(0, newlinePos + 1);

			command cmd;
			if (parseCommand(fullMsg, cmd))
				commandHandler(cmd, client);

			std::cout << "Server: Received: " << fullMsg.length()
					  << " bytes: " << fullMsg << std::endl;

			newlinePos = client.getReadBuffer().find('\n');
		}
		if (client.getReadBuffer().length() >= MAX_MSG_LEN)
		{
			std::cerr << "Server: Message max size exceeded (" << MAX_MSG_LEN
					  << " bytes maximum)." << std::endl;
			return false;
		}
	}
	return true;
}

void Server::queueMessage(User &client, const std::string &message)
{
	if (message.size() > MAX_WRITE_BUFFER ||
		client.getWriteBuffer().size() > MAX_WRITE_BUFFER - message.size())
	{
		client.setShouldDisconnect();
		std::cerr << "Server: write buffer max size exceeded ("
				  << MAX_WRITE_BUFFER << " bytes maximum)." << std::endl;
		return;
	}

	client.getWriteBuffer() += message;

	for (std::size_t i = 1; i < _pollfds.size(); i++)
	{
		if (_pollfds[i].fd == client.getFd())
		{
			_pollfds[i].events |= POLLOUT;
			return;
		}
	}
}

void Server::broadcastToChannel(const Channel &channel, const std::string &message, const User *excludeUser)
{
	const std::set<User *> &users = channel.getUsers();

	for (std::set<User *>::const_iterator it = users.begin(); it != users.end(); it++)
	{
		if (excludeUser != nullptr && *it == excludeUser)
			continue;
		queueMessage(
					**it, message);
	}
}

void Server::broadcastToUserChannels(User &client, const std::string &message, const User *excludeUser)
{
	std::set<User *> alreadySent;

	for (std::unordered_map<std::string, Channel>::iterator channelIt = _channels.begin(); channelIt != _channels.end(); channelIt++)
	{
		Channel &channel = channelIt->second;

		if (!channel.isUserInChannel(client))
			continue;

		const std::set<User *> &users = channel.getUsers();

		for (std::set<User *>::const_iterator userIt = users.begin(); userIt != users.end(); userIt++)
		{
			User &user = **userIt;

			if (excludeUser != nullptr && &user == excludeUser)
				continue;
			if (alreadySent.count(&user))
				continue;

			queueMessage(user, message);
			alreadySent.insert(&user);
		}
	}

}


bool Server::writeToClient(User &client, pollfd &clientPollfd)
{
	std::string &writeBuffer = client.getWriteBuffer();

	if (writeBuffer.empty())
	{
		clientPollfd.events &= ~POLLOUT;
		return true;
	}

	ssize_t bytesSent =
		send(client.getFd(), writeBuffer.c_str(), writeBuffer.size(), 0);

	if (bytesSent < 0)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			return true;

		std::cerr << "Server: Failed to send: " << client.getFd() << " : "
				  << std::strerror(errno) << std::endl;
		return false;
	}

	writeBuffer.erase(0, bytesSent);

	if (writeBuffer.empty())
		clientPollfd.events &= ~POLLOUT;

	return true;
}

void Server::setNonBlocking(int fd)
{
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		throw std::runtime_error(
			std::string("Server: non blocking flags set failed: ") +
			std::strerror(errno));
}

void Server::removeClient(std::size_t i)
{
	int fd = _pollfds[i].fd;
	User &user = _users.at(fd);

	_pollfds[i] = _pollfds.back();
	_pollfds.pop_back();

	removeUserFromAllChannels(user);
	_users.erase(fd);

	std::cout << "Server: Removed client at fd: " << fd << std::endl;
}

void Server::handleCap(const command &cmd, User &client)
{
	if (client.getIsRegistered())
		return;

	if (!cmd.vals.empty() && cmd.vals[0] == "LS")
	{
		client.setCapInProgress(true);
		queueMessage(client, msgCap(client));
		return;
	}
	if (!cmd.vals.empty() && cmd.vals[0] == "END")
	{
		client.setCapInProgress(false);
		registerClient(client);
		return;
	}
}

void Server::loop()
{
	while (true)
	{
		int ready = poll(_pollfds.data(), _pollfds.size(), -1);

		if (ready == -1)
		{
			if (errno == EINTR)
				continue;

			throw std::runtime_error(std::string("Server: Poll failed: ") +
									 std::strerror(errno));
		}

		if (_pollfds[0].revents & POLLIN)
			acceptClients();

		for (std::size_t i = 1; i < _pollfds.size();)
		{
			int fd = _pollfds[i].fd;
			short events = _pollfds[i].revents;
			bool keepClient = true;

			if (events & POLLIN)
			{
				User &client = _users.at(fd);
				keepClient = readClient(client);
			}
			if (keepClient && (events & POLLOUT))
			{
				User &client = _users.at(fd);
				keepClient = writeToClient(client, _pollfds[i]);
			}

			if (_users.at(fd).getShouldDisconnect())
				keepClient = false;

			if (events & (POLLERR | POLLHUP | POLLNVAL))
				keepClient = false;

			if (!keepClient)
				removeClient(i);
			else
				i++;
		}
	}
}

bool Server::commandHandler(const command &cmd, User &client)
{
	if (cmd.key == "CAP")
	{
		handleCap(cmd, client);
		return true;
	}
	if (cmd.key == "PASS")
	{
		handlePass(cmd, client);
		return true;
	}
	if (cmd.key == "USER")
	{
		handleUser(cmd, client);
		return true;
	}
	if (cmd.key == "NICK")
	{
		handleNick(cmd, client);
		return true;
	}
	if (cmd.key == "PING")
	{
		handlePing(cmd, client);
		return true;
	}

	if (!client.getIsRegistered())
	{
		queueMessage(client, msgReply(client, ERR_NOTREGISTERED));
		return false;
	}

	if (cmd.key == "PART")
	{
		handlePart(cmd, client);
		return true;
	}
	if (cmd.key == "PRIVMSG")
	{
		handlePrivMsg(cmd, client);
		return true;
	}

	return false;
}

Channel *Server::getChannel(const std::string &name)
{
	for (std::unordered_map<std::string, Channel>::iterator it =
			 _channels.begin();
		 it != _channels.end(); it++)
	{
		if (lowerCaseEqual(it->second.getName(), name))
		{
			Channel &channel = it->second;
			return &channel;
		}
	}
	return nullptr;
}

void Server::removeUserFromAllChannels(User &user)
{
	for (std::unordered_map<std::string, Channel>::iterator it =
			 _channels.begin();
		 it != _channels.end();)
	{
		Channel &channel = it->second;
		channel.removeUser(user);
		if (channel.getUsers().empty())
			it = _channels.erase(it);
		else
			it++;
	}
}

Channel &Server::addChannel(const std::string &name)
{
	Channel &newChannel = _channels[name];
	newChannel.setName(name);
	return newChannel;
}

void Server::removeChannel(const Channel &channel)
{
	_channels.erase(channel.getName());
}

User *Server::getUser(const std::string &nickName)
{
	for (std::unordered_map<int, User>::iterator it = _users.begin();
		 it != _users.end(); it++)
	{
		if (lowerCaseEqual(it->second.getNickName(), nickName))
			return &it->second;
	}
	return nullptr;
}
