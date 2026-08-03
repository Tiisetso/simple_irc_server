#pragma once

#include <poll.h>
#include <unistd.h>

#include <csignal>
#include <ctime>
#include <string>
#include <unordered_map>
#include <vector>

#include "Channel.hpp"
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
		std::unordered_map<std::string, Channel> _channels;

		std::string _port;
		std::string _password;
		std::string _version = "ircserv-0.1";
		std::time_t _startedAt;
		std::string _createdAt;

		static volatile sig_atomic_t _exitServer;
		static void handleSignal(int sig);

		void queueMessage(User &client, const std::string &message);
		void broadcastToChannel(const Channel &channel,
								const std::string &message,
								const User *excludeUser);
		void broadcastToUserChannels(User &client, const std::string &message,
									 const User *excludeUser);

		void acceptClients();
		void readClient(User &client);
		void writeToClient(User &client, pollfd &clientPollfd);
		void removeClient(std::size_t i);
		void registerClient(User &client);
		void sendWelcome(User &client);

		User *getUser(const std::string &nickName);

		Channel *getChannel(const std::string &name);
		Channel &addChannel(const std::string &name);
		void removeChannel(const Channel &channel);
		void removeUserFromAllChannels(User &client);
		std::size_t countUserChannels(User &client);

		static void setNonBlocking(int fd);

		bool commandHandler(const command &cmd, User &client);
		void handleCap(const command &cmd, User &client);
		void handlePass(const command &cmd, User &client);
		void handleUser(const command &cmd, User &client);
		void handleNick(const command &cmd, User &client);
		bool nickInUse(const std::string &val, User &client);
		void handlePing(const command &cmd, User &client);
		void handlePart(const command &cmd, User &client);
		void handleJoin(const command &cmd, User &client);
		void handleKick(const command &cmd, User &client);
		void kickSingleUserFromChannel(User &kicker, Channel &channel,
									   const std::string &targetNick,
									   const std::string &reason);
		void handlePrivMsg(const command &cmd, User &client);
		void handleQuit(const command &cmd, User &client);
		void handleInvite(const command &cmd, User &client);
		void handleMode(const command &cmd, User &client);
		void handleUserMode(const command &cmd, User &client,
							const std::string &target);
		void handleChannelMode(const command &cmd, User &client,
							   const std::string &target);
		void parseChannelMode(const command &cmd, User &client,
							  Channel &channel);
		void handleModeL(User &client, Channel &channel, char sign,
						 const std::string &argument);

		std::string msgPrefix(const User &client);
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
		std::string msgCap(const User &client);
		std::string msgMode(const User &client, const Channel &channel);
		std::string msgInvalidModeParam(const User &client,
										const Channel &channel,
										const std::string &modeChar,
										const std::string &parameter,
										const std::string &description);

	public:
		Server() = delete;
		Server(const std::string &port, const std::string &password);
		Server(const Server &other) = delete;
		Server &operator=(const Server &other) = delete;
		~Server();

		void createSocket();
		void loop();
		void signalSetup();
};
