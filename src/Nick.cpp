#include <iostream>
#include <string>

#include "Server.hpp"

/*
ERR_NONICKNAMEGIVEN (431)
  "<client> :No nickname given"
Returned when a nickname parameter is expected for a command but isn’t given.

ERR_ERRONEUSNICKNAME (432)
  "<client> <nick> :Erroneus nickname"
Returned when a NICK command cannot be successfully completed as the desired
nickname contains characters that are disallowed by the server. See the NICK
command for more information on characters which are allowed in various IRC
servers. The text used in the last param of this message may vary.

ERR_NICKNAMEINUSE (433)
  "<client> <nick> :Nickname is already in use"
*/
/*
MUST allow at least all alphanumerical characters,
square and curly brackets ([]{}), backslashes (\), and pipe (|) characters in
nicknames,
*/

/*
16:46 -NickServ(NickServ@services.libera.chat)- This nickname is
		  registered. Please choose a different nickname, or identify
		  via /msg NickServ IDENTIFY yuan <password>
16:47 -!- No nickname given
16:47 -!- You're now known as jizn

*/

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

bool Server::isValidNickname(const std::string &val)
{
	if (val.empty() || val.size() > 30)
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

bool Server::nickNameInUse(const std::string &val, User &client)
{
	for (std::unordered_map<int, User>::const_iterator it = _users.begin();
		 it != _users.end(); it++)
	{
		if (lowerCaseEqual(it->second.getNickName(), val) &&
			it->second.getFd() != client.getFd())
			return true;
	}

	return false;
}

void Server::handleNick(const command &cmd, User &client)
{
	if (cmd.vals.empty() || cmd.vals[0].empty())
	{
		queueMessage(client, msgFormat("*", ERR_NONICKNAMEGIVEN));
		return;
	}
	if (!isValidNickname(cmd.vals[0]))
	{
		queueMessage(client, msgFormat("*", ERR_ERRONEUSNICKNAME, cmd.vals[0]));
		return;
	}
	if (nickNameInUse(cmd.vals[0], client))
	{
		queueMessage(client, msgFormat("*", ERR_NICKNAMEINUSE, cmd.vals[0]));
		return;
	}
	std::cout << "nick key: " << cmd.key << std::endl;

	for (size_t i = 0; i < cmd.vals.size(); i++)
	{
		std::cout << "nick val: " << cmd.vals[i] << std::endl;
	}
}

