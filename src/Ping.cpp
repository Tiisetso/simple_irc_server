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

	std::string pongReply = ":" + _serverName + " PONG " + _serverName + " :" +
							cmd.vals[0] + "\r\n";
	queueMessage(client, pongReply);
}
