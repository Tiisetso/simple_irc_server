#include <algorithm>
#include <iostream>

#include "ReplyError.hpp"
#include "Server.hpp"

#define MAX_USERNAME_LEN 12

void Server::handlePass(const command &cmd, User &client)
{
	if (cmd.vals.empty())
	{
		queueMessage(client, msgReply(client, ERR_NEEDMOREPARAMS, "PASS"));
		return;
	}

	if (client.getIsRegistered())
	{
		queueMessage(client, msgReply(client, ERR_ALREADYREGISTERED));
		return;
	}

	if (cmd.vals[0] != _password)
	{
		client.setPassMatch(false);
		queueMessage(client, msgReply(client, ERR_PASSWDMISMATCH));
		return;
	}
	client.setPassMatch(true);
	registerClient(client);
}

void Server::handleUser(const command &cmd, User &client)
{
	if (cmd.vals.size() < 4)
	{
		queueMessage(client, msgReply(client, ERR_NEEDMOREPARAMS, "USER"));
		return;
	}

	if (client.getIsRegistered())
	{
		queueMessage(client, msgReply(client, ERR_ALREADYREGISTERED));
		return;
	}

	std::string username = cmd.vals[0];
	std::replace(username.begin(), username.end(), '@', '_');

	if (username.size() > MAX_USERNAME_LEN)
		username.resize(MAX_USERNAME_LEN);

	client.setUserName(username);
	client.setRealName(cmd.vals[3]);
	registerClient(client);
}

void Server::registerClient(User &client)
{
	if (client.getIsRegistered())
		return;
	if (!client.getPassMatch())
		return;
	if (client.getNickName().empty())
		return;
	if (client.getUserName().empty())
		return;
	client.setIsRegistered();
	sendWelcome(client);
	std::cout << "Client is registered!" << std::endl;
}

void Server::sendWelcome(User &client)
{
	std::string _createdAt = "2056-07-23";

	std::string prefix = msgPrefix(client);

	// 001
	queueMessage(client, msgNumeric(client, 1, "",
									"Welcome to the " + _serverName +
										" Network, " + prefix));
	// 002
	queueMessage(client, msgNumeric(client, 2, "",
									"Your host is " + _serverName +
										", running version " + _version));
	// 003
	queueMessage(client, msgNumeric(client, 3, "",
									"This server was created " + _createdAt));

	// 004
	queueMessage(
		client,
		msgNumeric(client, 4, _serverName + " " + _version + " o itkol", ""));
}
