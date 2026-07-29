#include "ReplyError.hpp"
#include "Server.hpp"
#include "Utilities.hpp"

#define MAX_TARGETS 5

void Server::handlePrivMsg(const command &cmd, User &client)
{
	if (cmd.vals.empty())
	{
		queueMessage(client, msgReply(client, ERR_NORECIPIENT));
		return;
	}

	if (cmd.vals.size() < 2 || cmd.vals[1].empty())
	{
		queueMessage(client, msgReply(client, ERR_NOTEXTTOSEND));
		return;
	}

	std::vector<std::string> recipientList;
	recipientList = splitStrToVec(cmd.vals[0], ',');
	if (recipientList.size() > MAX_TARGETS)
	{
		queueMessage(client, msgReply(client, ERR_TOOMANYTARGETS));
		return;
	}

	std::string msg = cmd.vals[1];

	for (std::size_t i = 0; i < recipientList.size(); i++)
	{
		std::string recipient = recipientList[i];
		if (recipient.empty())
			continue;

		// TODO: improve this depending on JOIN's implementation.
		if (recipient[0] == '#')
		{
			Channel *channel = getChannel(recipient);
			if (!channel)
				queueMessage(client,
							 msgReply(client, ERR_NOSUCHCHANNEL, recipient));
			else
			{
				if (channel->isUserInChannel(client))
				{
					const std::string message =
						msgFromClient(client, cmd.key, channel->getName(), msg);
					broadcastToChannel(*channel, message, &client);
				}
				else
					queueMessage(client, msgReply(client, ERR_CANNOTSENDTOCHAN,
												  recipient));
			}
		}
		else
		{
			User *user = getUser(recipient);
			if (!user || user->getIsRegistered() == false)
				queueMessage(client,
							 msgReply(client, ERR_NOSUCHNICK, recipient));
			else
			{
				queueMessage(*user, msgFromClient(client, cmd.key,
												  user->getNickName(), msg));
			}
		}
	}
}
