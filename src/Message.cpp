#include "ReplyError.hpp"
#include "Server.hpp"

std::string Server::msgTarget(const User &client)
{
	if (client.getNickName().empty())
		return "*";
	return client.getNickName();
}

std::string Server::msgFormat(const User &client, errReplyCode errorCode)
{
	return ":" + _serverName + " " + std::to_string(errorCode) + " " +
		   msgTarget(client) + " :" + errReplyMsg.at(errorCode) + "\r\n";
}

std::string Server::msgFormat(const User &client, errReplyCode errorCode,
							  const std::string &prefix)
{
	return ":" + _serverName + " " + std::to_string(errorCode) + " " +
		   msgTarget(client) + " " + prefix + " :" + errReplyMsg.at(errorCode) +
		   "\r\n";
}

std::string Server::msgPrefix(const User &client)
{
	return msgTarget(client) + "!" + client.getUserName() + "@" +
		   client.getHost();
}

std::string Server::msgFromClient(const User &client,
								  const std::string &command,
								  const std::string &params)
{
	return ":" + msgPrefix(client) + " " + command + " " + params + "\r\n";
}

std::string Server::msgFromServer(const std::string &command,
								  const std::string &trailing)
{
	return ":" + _serverName + " " + command + " " + _serverName + " :" +
		   trailing + "\r\n";
}