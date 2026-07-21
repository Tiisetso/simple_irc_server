#include <cctype>
#include <string>

#include "Server.hpp"

#define MAX_NICK_LEN 30

static bool lowerCaseEqual(const std::string &a, const std::string &b)
{
	if (a.size() != b.size())
		return false;

	for (size_t i = 0; i < a.size(); i++)
	{
		char left = a[i];
		char right = b[i];

		if (left >= 'A' && left <= 'Z')
			left += 'a' - 'A';
		if (right >= 'A' && right <= 'Z')
			right += 'a' - 'A';

		if (left != right)
			return false;
	}

	return true;
}

bool Server::isValidNick(const std::string &val)
{
	if (val.empty() || val.size() > MAX_NICK_LEN)
		return false;

	for (size_t i = 0; i < val.size(); i++)
	{
		if (!(std::isalnum(static_cast<unsigned char>(val[i])) ||
			  val[i] == '-' || val[i] == '_' || val[i] == '[' ||
			  val[i] == ']' || val[i] == '{' || val[i] == '}' ||
			  val[i] == '\\' || val[i] == '|'))
			return false;
	}

	return true;
}

bool Server::nickInUse(const std::string &val, User &client)
{
	for (std::unordered_map<int, User>::const_iterator it = _users.begin();
		 it != _users.end(); it++)
	{
		if (it->second.getFd() != client.getFd() &&
			lowerCaseEqual(it->second.getNickName(), val))
			return true;
	}

	return false;
}

void Server::handleNick(const command &cmd, User &client)
{
	std::string target{};

	if (client.getNickName().empty())
		target = "*";
	else
		target = client.getNickName();

	if (cmd.vals.empty() || cmd.vals[0].empty())
	{
		queueMessage(client, msgFormat(target, ERR_NONICKNAMEGIVEN));
		return;
	}
	if (!isValidNick(cmd.vals[0]))
	{
		queueMessage(client,
					 msgFormat(target, ERR_ERRONEUSNICKNAME, cmd.vals[0]));
		return;
	}
	if (client.getNickName() == cmd.vals[0])
		return;
	if (nickInUse(cmd.vals[0], client))
	{
		queueMessage(client, msgFormat(target, ERR_NICKNAMEINUSE, cmd.vals[0]));
		return;
	}
	if (!client.getIsRegistered())
	{
		client.setNickName(cmd.vals[0]);
		// TODO: try to register
		return;
	}

	const std::string nickMessage = ":" + target + "!" + client.getUserName() +
									"@" + client.getHost() + " NICK " + cmd.vals[0] +
									"\r\n";
	client.setNickName(cmd.vals[0]);
	queueMessage(client, nickMessage);
	// TODO: inform others sharing channels with this client about the change
}
