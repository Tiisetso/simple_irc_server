#include "ReplyError.hpp"
#include "Server.hpp"

void Server::handlePing(const command &cmd, User &client)
{
	if (cmd.vals.empty())
	{
		queueMessage(client, msgFormat(client, ERR_NEEDMOREPARAMS, "PING"));
		return;
	}
	if (cmd.vals[0].empty())
	{
		queueMessage(client, msgFormat(client, ERR_NOORIGIN));
		return;
	}

	std::string pongReply = msgFromServer("PONG", cmd.vals[0]);
	queueMessage(client, pongReply);
}
