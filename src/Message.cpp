#include "ReplyError.hpp"
#include "Server.hpp"

static std::string msgTarget(const User &client)
{
	if (client.getNickName().empty())
		return "*";
	return client.getNickName();
}

/**
 * @brief Builds a generic IRC message.
 *
 * Constructs a message in the format:
 * `:<prefix> <command> [params] [:trailing]\r\n`
 *
 * @param prefix Message prefix (server name or client prefix).
 * @param command IRC command or numeric reply.
 * @param params Optional parameters.
 * @param trailing Optional trailing parameter.
 * @return Formatted IRC message.
 *
 * @example
 * :ircserv PING yuan :hello\r\n
 */
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

/**
 * @brief Builds a client's IRC prefix.
 *
 * The prefix has the form:
 * `<nickname>!<username>@<host>`
 *
 * @param client Client whose prefix is generated.
 * @return Client prefix.
 *
 * @example
 * yuan!juan@127.0.0.1
 */
std::string Server::msgPrefix(const User &client)
{
	return msgTarget(client) + "!" + client.getUserName() + "@" +
		   client.getHost();
}

/**
 * @brief Builds a numeric error reply from the server.
 *
 * @param client Client receiving the reply.
 * @param codeReply IRC numeric reply code.
 * @param prefix Optional extra parameter before the trailing message.
 * @return Formatted numeric error reply.
 *
 * @example
 * :ircserv 433 yuan yuan :Nickname is already in use\r\n
 */
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

/**
 * @brief Builds a message sent by the server.
 *
 * The server name is used as both the message prefix and the first parameter.
 *
 * @param command IRC command.
 * @param trailing Trailing parameter.
 * @return Formatted server message.
 *
 * @example
 * :ircserv PONG ircserv :12345\r\n
 */
std::string Server::msgFromServer(const std::string &command,
								  const std::string &trailing)
{
	return msgBase(_serverName, command, _serverName, trailing);
}

/**
 * @brief Builds a message whose source is a client.
 *
 * @param client Client sending the message.
 * @param command IRC command.
 * @param params Optional command parameters.
 * @param trailing Optional trailing parameter.
 * @return Formatted client message.
 *
 * @example
 * :yuan!juan@127.0.0.1 NICK :newnick\r\n
 */
std::string Server::msgFromClient(const User &client,
								  const std::string &command,
								  const std::string &params,
								  const std::string &trailing)
{
	return msgBase(msgPrefix(client), command, params, trailing);
}

/**
 * @brief Builds a numeric reply from the server.
 *
 * Numeric reply codes are formatted as three digits (e.g. 001, 433).
 *
 * @param client Client receiving the reply.
 * @param code IRC numeric reply code.
 * @param middle Optional parameters between the nickname and trailing text.
 * @param trailing Trailing message.
 * @return Formatted numeric reply.
 *
 * @example
 * :ircserv 001 yuan :Welcome\r\n
 */
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

std::string Server::msgCap(const User &client)
{
	return ":" + _serverName + " CAP " + msgTarget(client) + " LS :\r\n";
}

std::string Server::msgMode(const User &client, const Channel &channel)
{
	return ":" + _serverName + " 324 " + client.getNickName() + " " +
		   channel.getName() + " " + channel.getMode() + "\r\n";
}

