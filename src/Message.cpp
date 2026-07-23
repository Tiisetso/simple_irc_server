#include "ReplyError.hpp"
#include "Server.hpp"

static std::string msgTarget(const User &client)
{
	if (client.getNickName().empty())
		return "*";
	return client.getNickName();
}

static std::string msgBase(const std::string &prefix,
						   const std::string &command,
						   const std::string &params,
						   const std::string &trailing)
{
	std::string trailingPiece;
	std::string paramsPiece;

	trailingPiece = trailing.empty() ? "" : " :" + trailing;
	paramsPiece = params.empty() ? "" : " " + params;

	return ":" + prefix + " " + command + paramsPiece + trailingPiece + "\r\n";
}

std::string Server::msgPrefix(const User &client)
{
	return msgTarget(client) + "!" + client.getUserName() + "@" +
		   client.getHost();
}

std::string Server::msgReply(const User &client, errReplyCode codeReply,
							 const std::string &prefix)
{
	std::string params;

	if (prefix.empty())
		params = msgTarget(client);
	else
		params = msgTarget(client) + " " + prefix;

	return msgBase(_serverName, std::to_string(codeReply), params,
				   errReplyMsg.at(codeReply));
}

std::string Server::msgFromServer(const std::string &command,
								  const std::string &trailing)
{
	return msgBase(_serverName, command, _serverName, trailing);
}

std::string Server::msgFromClient(const User &client,
								  const std::string &command,
								  const std::string &params,
								  const std::string &trailing)
{
	return msgBase(msgPrefix(client), command, params, trailing);
}

std::string Server::msgNumeric(const User &client, int code,
							   const std::string &middle,
							   const std::string &trailing)
{
	std::string params = msgTarget(client);
	if (!middle.empty())
		params += " " + middle;
	return msgBase(_serverName, std::to_string(code), params, trailing);
}
