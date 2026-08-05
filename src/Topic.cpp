#include "ReplyError.hpp"
#include "Server.hpp"

void Server::handleTopic(const command &cmd, User &client)
{
	if (cmd.vals.empty())
	{
		queueMessage(client, msgReply(client, ERR_NEEDMOREPARAMS, cmd.key));
		return;
	}

	const std::string &channelName = cmd.vals[0];
	Channel *channel = getChannel(channelName);
	if (!channel)
	{
		queueMessage(client, msgReply(client, ERR_NOSUCHCHANNEL, channelName));
		return;
	}

	if (!channel->isUserInChannel(client))
	{
		queueMessage(client,
					 msgReply(client, ERR_NOTONCHANNEL, channel->getName()));
		return;
	}

	if (cmd.vals.size() >= 2)
	{
		const std::string &topic = cmd.vals[1];
		channel->setTopic(topic);
		const std::string topicMsg = ":" + msgPrefix(client) + " TOPIC " +
									 channel->getName() + " :" + topic +
									 "\r\n";
		broadcastToChannel(*channel, topicMsg, nullptr);
		return;
	}

	if (channel->getTopic().empty())
		queueMessage(client, msgNumeric(client, 331, channel->getName(),
										"No topic is set"));
	else
		queueMessage(client, msgNumeric(client, 332, channel->getName(),
										channel->getTopic()));
}
