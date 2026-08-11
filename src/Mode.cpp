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
		broadcastModeToChannel(client, channel, sign, 'o', user->getNickName());
	}
	if (sign == '+')
	{
		if (channel.isOperator(*user))
			return;
		channel.addOperator(*user);
		broadcastModeToChannel(client, channel, sign, 'o', user->getNickName());
	}
}

void Server::handleModeI(User &client, Channel &channel, char sign)
{
	if (sign == '+')
		channel.setInviteOnly(true);
	else
		channel.setInviteOnly(false);
	broadcastModeToChannel(client, channel, sign, 'i');
}

void Server::handleModeT(User &client, Channel &channel, char sign)
{
	if (sign == '+')
		channel.setTopicRestricted(true);
	else
		channel.setTopicRestricted(false);
	broadcastModeToChannel(client, channel, sign, 't');
}

void Server::handleModeL(User &client, Channel &channel, char sign,
						 const std::string &argument)
{
	if (sign == '-')
	{
		channel.removeLimit();
		broadcastModeToChannel(client, channel, sign, 'l');
		return;
	}
	if (argument.empty())
	{
		queueMessage(client, msgNumeric(client, 696, channel.getName() + " l",
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
		broadcastModeToChannel(client, channel, sign, 'l',
							   std::to_string(limit));
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
			queueMessage(client,
						 msgReply(client, ERR_INVALIDKEY, channel.getName()));
			return;
		}

		channel.setKey(argument);
		broadcastModeToChannel(client, channel, sign, 'k', argument);
	}
	if (sign == '-')
	{
		if (!channel.hasKey())
			return;
		else
			channel.removeKey();
		broadcastModeToChannel(client, channel, sign, 'k');
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
				handleModeT(client, channel, sign);
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

void Server::broadcastModeToChannel(User &client, Channel &channel, char sign,
									char mode, const std::string &argument)
{
	std::string params = channel.getName() + " " + sign + mode;
	if (!argument.empty())
		params += " " + argument;
	broadcastToChannel(channel, msgFromClient(client, "MODE", params), nullptr);
}
