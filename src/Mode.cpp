#include <cstddef>
#include <string>

#include "Channel.hpp"
#include "ReplyError.hpp"
#include "Server.hpp"

void Server::handleModeK(User &client, Channel &channel, char sign,
						 const std::string &argument)
{
	if (sign == '+')
	{
		// MODE #channel +K :
		if (argument.empty())
		{
			queueMessage(client, msgReply(client, ERR_INVALIDKEY, channel.getName()));
			return;
		}

		channel.setKey(argument);
		broadcastToChannel(channel,
						   msgFromClient(client, "MODE",
										 channel.getName() + " +k " + argument),
						   nullptr);
	}
	if (sign == '-')
	{
		if (!channel.hasKey())
			return;
		else
			channel.removeKey();
		broadcastToChannel(
			channel, msgFromClient(client, "MODE", channel.getName() + " -k"),
			nullptr);
	}
}

void Server::parseChannelMode(const command &cmd, User &client,
							  Channel &channel)
{
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
			continue;
		}

		// mode needs argument
		mode = c;
		std::string argument{};
		if (mode == 'o' || ((mode == 'k' || mode == 'l') && sign == '+'))
		{
			// missing argument
			if (argumentIndex >= cmd.vals.size())
			{
				queueMessage(client,
							 msgReply(client, ERR_NEEDMOREPARAMS, cmd.key));
				return;
			}
			argument = cmd.vals[argumentIndex];
			argumentIndex++;
		}

		switch (mode)
		{
			case 'i':
				// handleModeI(client, channel);
				break;
			case 't':
				// handleModeT(client, channel);
				break;
			case 'k':
				handleModeK(client, channel, sign, argument);
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
		bool isMember = channel->isUserInChannel(client);
		queueMessage(client, msgMode(client, *channel, isMember));
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
