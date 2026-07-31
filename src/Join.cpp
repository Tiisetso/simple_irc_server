#include <sstream>

#include "ReplyError.hpp"
#include "Server.hpp"
#include "Utilities.hpp"
#include "Validation.hpp"

#define MAX_CHANNELS_PER_USER 5

static std::string buildNamesList(const Channel &channel)
{
	std::ostringstream names;
	const std::set<User *> &users = channel.getUsers();

	for (std::set<User *>::const_iterator it = users.begin(); it != users.end();
		 ++it)
	{
		if (it != users.begin())
			names << ' ';
		if (channel.isOperator(**it))
			names << '@';
		names << (*it)->getNickName();
	}
	return names.str();
}

void Server::handleJoin(const command &cmd, User &client)
{
	if (cmd.vals.empty())
	{
		queueMessage(client, msgReply(client, ERR_NEEDMOREPARAMS, cmd.key));
		return;
	}

	std::vector<std::string> channelList = splitStrToVec(cmd.vals[0], ',');
	std::vector<std::string> keyList;
	if (cmd.vals.size() > 1)
		keyList = splitStrToVec(cmd.vals[1], ',');

	for (std::size_t i = 0; i < channelList.size(); ++i)
	{
		std::string channelName = channelList[i];
		if (channelName.empty())
			continue;

		if (!isValidChannelName(channelName))
		{
			queueMessage(client,
						 msgReply(client, ERR_BADCHANMASK, channelName));
			continue;
		}

		Channel *channel = getChannel(channelName);

		if (channel && channel->isUserInChannel(client))
			continue;

		if (countUserChannels(client) >= MAX_CHANNELS_PER_USER)
		{
			queueMessage(client,
						 msgReply(client, ERR_TOOMANYCHANNELS, channelName));
			continue;
		}
		if (!channel)
			channel = &addChannel(channelName);

		if (channel->isInviteOnly() && !channel->isUserInvited(client))
		{
			queueMessage(client,
						 msgReply(client, ERR_INVITEONLYCHAN, channelName));
			continue;
		}

		std::string key = (i < keyList.size()) ? keyList[i] : "";
		if (channel->hasKey() && channel->getKey() != key)
		{
			queueMessage(client,
						 msgReply(client, ERR_BADCHANNELKEY, channelName));
			continue;
		}

		if (channel->isLimitReached())
		{
			queueMessage(client,
						 msgReply(client, ERR_CHANNELISFULL, channelName));
			continue;
		}

		const bool firstMember = channel->getUsers().empty();
		channel->addUser(client);
		if (firstMember)
			channel->addOperator(client);

		const std::string joinMsg =
			msgFromClient(client, "JOIN", channel->getName());
		broadcastToChannel(*channel, joinMsg, nullptr);

		const std::string namesList = buildNamesList(*channel);
		queueMessage(client, msgNumeric(client, 353, "= " + channel->getName(),
										namesList));
		queueMessage(client, msgNumeric(client, 366, channel->getName(),
										"End of /NAMES list"));
	}
}
