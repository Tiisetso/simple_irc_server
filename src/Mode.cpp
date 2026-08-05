#include <climits>
#include <cstddef>
#include <string>

#include "Channel.hpp"
#include "ReplyError.hpp"
#include "Server.hpp"

void Server::handleModeO(User &client, Channel &channel, char sign,
						 const std::string &argument)
{
	User *user = getUser(argument);

	if (!user || !user->getIsRegistered())
	{
		queueMessage(client, msgReply(client, ERR_NOSUCHNICK, argument));
		return;
	}
	if (!channel.isUserInChannel(*user))
	{
		queueMessage(client,
					 msgReply(client, ERR_USERNOTINCHANNEL,
							  user->getNickName() + " " + channel.getName()));
		return;
	}
	if (sign == '-')
	{
		if (!channel.isOperator(*user))
			return;
		channel.removeOperator(*user);
		broadcastToChannel(
			channel,
			msgFromClient(client, "MODE",
						  channel.getName() + " -o " + user->getNickName()),
			nullptr);
	}
	if (sign == '+')
	{
		if (channel.isOperator(*user))
			return;
		channel.addOperator(*user);
		broadcastToChannel(
			channel,
			msgFromClient(client, "MODE",
						  channel.getName() + " +o " + user->getNickName()),
			nullptr);
	}
}

void Server::handleModeI(User &client, Channel &channel, char sign)
{
	if (sign == '+')
	{
		channel.setInviteOnly(true);
		broadcastToChannel(
			channel, msgFromClient(client, "MODE", channel.getName() + " +i "),
			nullptr);
	}
	else
	{
		channel.setInviteOnly(false);
		broadcastToChannel(
			channel, msgFromClient(client, "MODE", channel.getName() + " -i "),
			nullptr);
	}
}

void Server::handleModeL(User &client, Channel &channel, char sign,
						 const std::string &argument)
{
	if (sign == '-')
	{
		channel.removeLimit();
		broadcastToChannel(
			channel, msgFromClient(client, "MODE", channel.getName() + " -l"),
			nullptr);
		return;
	}
	if (argument.empty())
	{
		queueMessage(client, msgNumeric(client, 696,
										channel.getName() + " l " + argument,
										"empty mode param"));
		return;
	}

	for (std::size_t i = 0; i < argument.size(); ++i)
	{
		if (argument[i] < '0' || argument[i] > '9')
		{
			queueMessage(
				client,
				msgNumeric(client, 696, channel.getName() + " l " + argument,
						   "invalid limit"));
			return;
		}
	}

	try
	{
		// string to unsigned long
		unsigned long number = std::stoul(argument);

		if (number == 0 || number > INT_MAX)
		{
			queueMessage(
				client,
				msgNumeric(client, 696, channel.getName() + " l " + argument,
						   "invalid limit"));
			return;
		}
		std::size_t limit = number;
		channel.setLimit(limit);
		broadcastToChannel(
			channel,
			msgFromClient(client, "MODE",
						  channel.getName() + " +l " + std::to_string(limit)),
			nullptr);
	}
	catch (...)
	{
		queueMessage(client, msgNumeric(client, 696,
										channel.getName() + " l " + argument,
										"invalid limit"));
		return;
	}
}

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
			queueMessage(client, msgReply(client, ERR_UNKNOWNMODE, modeChar));
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
				handleModeI(client, channel, sign);
				break;
			case 't':
				// handleModeT(client, channel);
				break;
			case 'k':
				handleModeK(client, channel, sign, argument);
				break;
			case 'o':
				handleModeO(client, channel, sign, argument);
				break;
			case 'l':
				handleModeL(client, channel, sign, argument);
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

	queueMessage(client, msgReply(client, ERR_UMODEUNKNOWNFLAG));
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
	if (!channel->isUserInChannel(client))
	{
		queueMessage(client, msgReply(client, ERR_NOTONCHANNEL, target));
		return;
	}
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
