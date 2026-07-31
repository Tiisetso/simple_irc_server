#include "ReplyError.hpp"
#include "Server.hpp"
#include "Utilities.hpp"

void Server::handleKick(const command &cmd, User &client)
{
	if (cmd.vals.size() < 2)
	{
		queueMessage(client, msgReply(client, ERR_NEEDMOREPARAMS, cmd.key));
		return;
	}

	const std::string &channelName = cmd.vals[0];
	std::string reason;
	if (cmd.vals.size() > 2)
		reason = cmd.vals[2];

	const std::vector<std::string> nickList = splitStrToVec(cmd.vals[1], ',');

	Channel *channel = getChannel(channelName);
	if (!channel)
	{
		queueMessage(client, msgReply(client, ERR_NOSUCHCHANNEL, channelName));
		return;
	}

	if (!channel->isUserInChannel(client))
	{
		queueMessage(client, msgReply(client, ERR_NOTONCHANNEL, channelName));
		return;
	}

	if (!channel->isOperator(client))
	{
		queueMessage(
			client, msgReply(client, ERR_CHANOPRIVSNEEDED, channel->getName()));
		return;
	}

	for (std::size_t i = 0; i < nickList.size(); ++i)
	{
		const std::string &nick = nickList[i];
		if (nick.empty())
			continue;

		kickSingleUserFromChannel(client, channelName, nick, reason);
	}
}

void Server::kickSingleUserFromChannel(User &kicker,
									   const std::string &channelName,
									   const std::string &targetNick,
									   const std::string &reason)
{
	Channel *channel = getChannel(channelName);
	if (!channel)
	{
		queueMessage(kicker, msgReply(kicker, ERR_NOSUCHCHANNEL, channelName));
		return;
	}

	User *victim = getUser(targetNick);
	if (!victim)
	{
		queueMessage(kicker, msgReply(kicker, ERR_NOSUCHNICK, targetNick));
		return;
	}

	if (!channel->isUserInChannel(*victim))
	{
		queueMessage(
			kicker, msgReply(kicker, ERR_USERNOTINCHANNEL,
							 victim->getNickName() + " " + channel->getName()));
		return;
	}

	const std::string kickParams =
		channel->getName() + " " + victim->getNickName();
	const std::string kickMsg =
		msgFromClient(kicker, "KICK", kickParams, reason);
	broadcastToChannel(*channel, kickMsg, nullptr);
	channel->removeUser(*victim);
	if (channel->getUsers().empty())
		removeChannel(*channel);
}
