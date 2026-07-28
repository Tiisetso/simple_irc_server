#include "ReplyError.hpp"
#include "Server.hpp"
#include "Utilities.hpp"
#include "Validation.hpp"

void Server::handlePart(const command &cmd, User &client)
{
	if (cmd.vals.empty())
	{
		queueMessage(client, msgReply(client, ERR_NEEDMOREPARAMS, cmd.key));
		return;
	}

	std::vector<std::string> channelList;
	channelList = splitStrToVec(cmd.vals[0], ',');

	std::string reason = "";
	if (cmd.vals.size() > 1)
		reason = cmd.vals[1];

	for (std::size_t i = 0; i < channelList.size(); i++)
	{
		std::string channelName = channelList[i];
		if (channelName.empty())
			continue;
		if (!isValidChannelName(channelName))
		{
			queueMessage(client, msgReply(client, ERR_NOSUCHCHANNEL, channelName));
			continue;
		}

		Channel *channel = getChannel(channelName);
		if (!channel)
			queueMessage(client,
						 msgReply(client, ERR_NOSUCHCHANNEL, channelName));
		else
		{
			if (channel->isUserInChannel(client))
			{
				// TODO: this queueMessage will be replaced by broadcast
				// message.
				queueMessage(
					client, msgFromClient(client, "PART", channelName, reason));
				channel->removeUser(client);
				if (channel->getUsers().empty())
					removeChannel(*channel);
			}
			else
				queueMessage(client,
							 msgReply(client, ERR_NOTONCHANNEL, channelName));
		}
	}
}
