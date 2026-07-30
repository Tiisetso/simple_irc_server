#include "Server.hpp"

void Server::handleInvite(const command &cmd, User &client)
{
	if(cmd.vals.size() < 2)
	{
		queueMessage(client, msgReply(client, ERR_NEEDMOREPARAMS, cmd.key));
		return;
	}

	std::string nickName = cmd.vals[0];
	User *invitee = getUser(nickName);
	if (!invitee || !invitee->getIsRegistered())
	{
		queueMessage(client, msgReply(client, ERR_NOSUCHNICK, cmd.key));
		return;	
	}

	std::string channelName = cmd.vals[1];
	Channel *channel = getChannel(channelName);
	if(!channel)
	{
		queueMessage(client,
						 msgReply(client, ERR_NOSUCHCHANNEL, channelName));
		return;
	}

	if(!channel->isUserInChannel(client))
	{
		queueMessage(client,
						 msgReply(client, ERR_NOTONCHANNEL, channel->getName()));
		return;
	}

	if(channel->isUserInChannel(*invitee))
	{
		queueMessage(client,
						 msgReply(client, ERR_USERONCHANNEL, invitee->getNickName() + " " + channel->getName()));
		return;
	}

	if (channel->isInviteOnly() && !channel->isOperator(client))
	{
		queueMessage(client,
						 msgReply(client, ERR_CHANOPRIVSNEEDED, channel->getName()));
		return;
	}

	channel->inviteUser(*invitee);
	queueMessage(client, msgNumeric(client, 341, invitee->getNickName() + " " + channel->getName(), ""));
	queueMessage(*invitee, msgFromClient(client, cmd.key, invitee->getNickName(), channel->getName()));
}