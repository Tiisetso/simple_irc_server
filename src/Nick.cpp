#include <cctype>
#include <string>

#include "Server.hpp"
#include "Utilities.hpp"
#include "Validation.hpp"

bool Server::nickInUse(const std::string &val, User &client)
{
	for (std::unordered_map<int, User>::const_iterator it = _users.begin();
		 it != _users.end(); it++)
	{
		if (it->second.getFd() != client.getFd() &&
			lowerCaseEqual(it->second.getNickName(), val))
			return true;
	}

	return false;
}

void Server::handleNick(const command &cmd, User &client)
{
	if (cmd.vals.empty() || cmd.vals[0].empty())
	{
		queueMessage(client, msgReply(client, ERR_NONICKNAMEGIVEN));
		return;
	}
	if (!isValidNick(cmd.vals[0]))
	{
		queueMessage(client,
					 msgReply(client, ERR_ERRONEUSNICKNAME, cmd.vals[0]));
		return;
	}
	if (client.getNickName() == cmd.vals[0])
		return;
	if (nickInUse(cmd.vals[0], client))
	{
		queueMessage(client, msgReply(client, ERR_NICKNAMEINUSE, cmd.vals[0]));
		return;
	}
	if (!client.getIsRegistered())
	{
		client.setNickName(cmd.vals[0]);
		registerClient(client);
		return;
	}

	const std::string nickMessage = msgFromClient(client, "NICK", cmd.vals[0]);
	client.setNickName(cmd.vals[0]);
	queueMessage(client, nickMessage);
	broadcastToUserChannels(client, nickMessage, &client);
}
