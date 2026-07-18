#pragma once

#include <poll.h>
#include <unistd.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "Parser.hpp"
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
		bool processClient(User &client);
		void removeClient(std::size_t i);

		void setNonBlocking(int fd);
		void loop();

		bool commandHandler(const command &cmd, User &client);
		void handlePass(const command &cmd, User &client);

		void quickSend(User &client, const std::string &message);
};
