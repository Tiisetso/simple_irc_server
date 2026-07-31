#include "ReplyError.hpp"
#include "Server.hpp"

void Server::handleKick(const command &cmd, User &client)
{
	if (cmd.vals.size() < 2)
	{
		queueMessage(client, msgReply(client, ERR_NEEDMOREPARAMS, cmd.key));
		return;
	}

	const std::string &channelName = cmd.vals[0];
	const std::string &targetNick = cmd.vals[1];
	std::string reason;
	if (cmd.vals.size() > 2)
		reason = cmd.vals[2];

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

	User *victim = getUser(targetNick);
	if (!victim)
	{
		queueMessage(client, msgReply(client, ERR_NOSUCHNICK, targetNick));
		return;
	}

	if (!channel->isUserInChannel(*victim))
	{
		queueMessage(
			client, msgReply(client, ERR_USERNOTINCHANNEL,
							 victim->getNickName() + " " + channel->getName()));
		return;
	}

	const std::string kickParams =
		channel->getName() + " " + victim->getNickName();
	const std::string kickMsg =
		msgFromClient(client, "KICK", kickParams, reason);
	broadcastToChannel(*channel, kickMsg, nullptr);
	channel->removeUser(*victim);
	if (channel->getUsers().empty())
		removeChannel(*channel);
}
