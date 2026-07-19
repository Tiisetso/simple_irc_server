#pragma once

#include <poll.h>
#include <unistd.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "User.hpp"

class Server
{
	private:
		int _servSockFd;
		std::vector<pollfd> _pollfds;
		std::unordered_map<int, User> _users;

		std::string _port;
		std::string _password;

	public:
		Server() = delete;
		Server(const std::string &port, const std::string &password);
		Server(const Server &other) = delete;
		Server &operator=(const Server &other) = delete;
		~Server();

		void createSocket();

		void acceptClients();
		bool readClient(User &client);
		void queueMessage(User &client, const std::string &message);
		bool writeToClient(User &client, pollfd &clientPollfd);
		void removeClient(std::size_t i);

		void setNonBlocking(int fd);
		void loop();
};
