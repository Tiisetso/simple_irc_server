#include "ReplyError.hpp"
#include "Server.hpp"

static std::string msgTarget(const User &client)
{
	if (client.getNickName().empty())
		return "*";
	return client.getNickName();
}

// Builds the common IRC message format:
//   		:<prefix> <command> [params] [:trailing]\r\n
// Returns: :ircserv PING yuan :hello\r\n
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

// Builds a full client prefix:
//   		<nickname>!<username>@<host>
// Returns: yuan!jin@127.0.0.1
std::string Server::msgPrefix(const User &client)
{
	return msgTarget(client) + "!" + client.getUserName() + "@" +
		   client.getHost();
}

/// Builds a numeric error reply from the server.
//   :ircserv 431 yuan :No nickname given\r\n
//  Returns: :ircserv 433 yuan yuan :Nickname is already in use\r\n
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

// Builds a message sent directly from the server.
//   		:<server-name> <command> <server-name> :<trailing>
// Returns: :ircserv PONG ircserv :12345\r\n
std::string Server::msgFromServer(const std::string &command,
								  const std::string &trailing)
{
	return msgBase(_serverName, command, _serverName, trailing);
}

// Builds a message whose source is a client.
//  		 :<nick>!<user>@<host> <command> [params] [:trailing]
// Returns:  :yuan!jin@127.0.0.1 NICK :newnick\r\n
std::string Server::msgFromClient(const User &client,
								  const std::string &command,
								  const std::string &params,
								  const std::string &trailing)
{
	return msgBase(msgPrefix(client), command, params, trailing);
}

// Builds a numeric server reply.
// Returns:  :ircserv 1 yuan :Welcome\r\n
std::string Server::msgNumeric(const User &client, int code,
							   const std::string &middle,
							   const std::string &trailing)
{
	std::string params = msgTarget(client);
	if (!middle.empty())
		params += " " + middle;
	std::string codeStr =
		code < 10 ? "00" + std::to_string(code) : std::to_string(code);
	return msgBase(_serverName, codeStr, params, trailing);
}
