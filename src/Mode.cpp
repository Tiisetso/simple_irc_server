#include <iostream>

#include "ReplyError.hpp"
#include "Server.hpp"

// MODE command dispatch and target validation
// Modestring parsing
// Implement the five required channel modes
// Build the successful-change broadcast
// Add MODE #channel query with 324

void Server::handleUserMode(const command &cmd, User &client,
							const std::string &target)
{
	User *user = getUser(target);

	if (!user || !user->getIsRegistered())
	{
		queueMessage(client, msgReply(client, ERR_NOSUCHNICK, target));
		return;
	}
	if (client.getFd() != user->getFd())
	{
		queueMessage(client, msgReply(client, ERR_USERSDONTMATCH));
		return;
	}

	// User mode not required
	if (cmd.vals.size() == 1)
	{
		queueMessage(client, msgNumeric(client, 221, ":", ""));
		return;
	}
}

void Server::handleChannelMode(const command &cmd, User &client,
							   const std::string &target)
{
	Channel *channel = getChannel(target);

	if (!channel)
	{
		queueMessage(client, msgReply(client, ERR_NOSUCHCHANNEL, target));
		return;
	}

	// MODE #channel
	if (cmd.vals.size() == 1)
	{
		// TODO: if (cmd.vals.size() == 1) reply with the channel's current
		// modes if channel mode is given.
		// queueMessage(client, queueMessage(client, msgNumeric(client, int
		// code, const std::string &middle, const std::string &trailing))) 324
		// 329
		return;
	}

	// MODE #channel <modestring>
	if (!channel->isOperator(client))
	{
		queueMessage(client, msgReply(client, ERR_CHANOPRIVSNEEDED, target));
		return;
	}
}

/*
Mode	+ argument	- argument
i	    none	    none
t	    none	    none
k	    key     	none
l	positive limit 	none
o	nickname    	nickname
*/

void Server::handleMode(const command &cmd, User &client)
{
	std::cout << "mode handler reached" << std::endl;
	if (cmd.vals.empty())
	{
		queueMessage(client, msgReply(client, ERR_NEEDMOREPARAMS));
		return;
	}

	const std::string &target = cmd.vals[0];

	if (target[0] != '#')
	{
		handleUserMode(cmd, client, target);
		return;
	}

	handleChannelMode(cmd, client, target);
}
