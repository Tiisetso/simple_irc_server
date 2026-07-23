#pragma once

#include <poll.h>
#include <unistd.h>

#include <string>
#include <unordered_map>
#include <vector>

#include "Parser.hpp"
#include "ReplyError.hpp"
#include "User.hpp"

class Server
{
	private:
		const std::string _serverName = "ircserv";
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

		bool commandHandler(const command &cmd, User &client);
		void handlePass(const command &cmd, User &client);
		void handleUser(const command &cmd, User &client);
		void handleNick(const command &cmd, User &client);
		void handlePing(const command &cmd, User &client);
		bool isValidNick(const std::string &val);
		bool nickInUse(const std::string &val, User &client);

		void registerClient(User &client);
		void sendWelcome(User &client);

		std::string msgReply(const User &client, errReplyCode codeReply,
							 const std::string &prefix = "");
		std::string msgFromClient(const User &client,
								  const std::string &command,
								  const std::string &params,
								  const std::string &trailing = "");
		std::string msgFromServer(const std::string &command,
								  const std::string &trailing = "");
		std::string msgNumeric(const User &client, int code,
							   const std::string &middle,
							   const std::string &trailing);
};
