#include <cstddef>
#include <iostream>
#include <string>

#include "Channel.hpp"
#include "ReplyError.hpp"
#include "Server.hpp"

void Server::handleModeI(Channel &channel, char sign)
{
	if (sign == '+')
		channel.setInviteOnly(true);
	else
		channel.setInviteOnly(false);
}

void Server::parseChannelMode(const command &cmd, User &client,
							  Channel &channel)
{
	(void)channel;

	char sign = '\0';
	char mode = '\0';
	const std::string &modeString = cmd.vals[1];
	std::size_t argumentIndex = 2;

	if (modeString.empty() || (modeString[0] != '+' && modeString[0] != '-'))
	{
		queueMessage(client, msgReply(client, ERR_UNKNOWNMODE, modeString));
		return;
	}

	for (std::size_t i = 0; i < modeString.size(); i++)
	{
		char c = modeString[i];

		if (c == '+' || c == '-')
		{
			sign = c;
			continue;
		}

		// unsupportedMode
		if (!(c == 'i' || c == 't' || c == 'k' || c == 'o' || c == 'l'))
		{
			std::string modeChar{c};
			queueMessage(client, msgReply(client, ERR_UNKNOWNMODE,
										  cmd.key + " " + modeChar));
			return;
		}

		// mode needs argument
		mode = c;
		std::string argument{};
		if (mode == 'o' || ((mode == 'k' || mode == 'l') && sign == '+'))
		{
			// missing argument
			if (argumentIndex >= cmd.vals.size())
			{
				std::string modeChar{c};
				queueMessage(client,
							 msgReply(client, ERR_NEEDMOREPARAMS, modeChar));
				return;
			}
			argument = cmd.vals[argumentIndex];
			argumentIndex++;
		}

		switch (mode)
		{
			case 'i':
				handleModeI(channel, sign);
				break;
			case 't':
				// handleModeT(client, channel);
				break;
			case 'k':
				// handlModeK(client, channel, sign, argument);
				break;
			case 'o':
				// handleModeO(client, channel, sign, argument);
				break;
			case 'l':
				// handleModeL(client, channel, sign, argument);
				break;
		}
	}
}

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
		queueMessage(client, msgNumeric(client, 221, cmd.key, ""));
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
		queueMessage(client, msgMode(client, *channel));
		return;
	}

	// MODE #channel <modestring>
	if (!channel->isOperator(client))
	{
		queueMessage(client, msgReply(client, ERR_CHANOPRIVSNEEDED, target));
		return;
	}
	parseChannelMode(cmd, client, *channel);
}

void Server::handleMode(const command &cmd, User &client)
{
	std::cout << "mode handler reached" << std::endl;
	if (cmd.vals.empty())
	{
		queueMessage(client, msgReply(client, ERR_NEEDMOREPARAMS, cmd.key));
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
