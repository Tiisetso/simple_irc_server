#include "Server.hpp"

#include <algorithm>
#include <iostream>

#include "ReplyError.hpp"

#define MAX_USERNAME_LEN 12

std::string Server::msgTarget(const User &client)
{
	if (client.getNickName().empty())
		return "*";
	return client.getNickName();
}

void Server::handlePass(const command &cmd, User &client)
{
	if (cmd.vals.empty())
	{
		queueMessage(client, msgFormat(client, ERR_NEEDMOREPARAMS, "PASS"));
		return;
	}

	if (client.getIsRegistered())
	{
		queueMessage(client, msgFormat(client, ERR_ALREADYREGISTERED));
		return;
	}

	if (cmd.vals[0] != _password)
	{
		client.setPassMatch(false);
		queueMessage(client, msgFormat(client, ERR_PASSWDMISMATCH));
		return;
	}
	client.setPassMatch(true);
	registerClient(client);
}

void Server::handleUser(const command &cmd, User &client)
{
	if (cmd.vals.size() < 4)
	{
		queueMessage(client, msgFormat(client, ERR_NEEDMOREPARAMS, "USER"));
		return;
	}

	if (client.getIsRegistered())
	{
		queueMessage(client, msgFormat(client, ERR_ALREADYREGISTERED));
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
	std::cout << "Client is registered!" << std::endl;
}

std::string Server::msgFormat(const User &client, errReplyCode errorCode)
{
	return ":" + _serverName + " " + std::to_string(errorCode) + " " +
		   msgTarget(client) + " :" + errReplyMsg.at(errorCode) + "\r\n";
}

std::string Server::msgFormat(const User &client, errReplyCode errorCode,
							  const std::string &prefix)
{
	return ":" + _serverName + " " + std::to_string(errorCode) + " " +
		   msgTarget(client) + " " + prefix + " :" + errReplyMsg.at(errorCode) +
		   "\r\n";
}
