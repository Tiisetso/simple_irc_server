#include "Validation.hpp"

#include <cctype>
#include <string>

#define MAX_PASS_LEN 32
#define MAX_CHANNELNAME_LEN 32
#define MAX_NICK_LEN 30

bool validPort(const std::string &portString)
{
	unsigned int port;
	char c;

	port = 0;
	if (portString.empty())
		return (false);
	for (std::size_t i = 0; i < portString.length(); ++i)
	{
		c = portString[i];
		if (c < '0' || c > '9')
			return (false);
		port = port * 10 + (c - '0');
		if (port > 65535)
			return (false);
	}
	return (port > 0);
}

bool validPassword(const std::string &password)
{
	if (password.empty())
		return false;
	if (password.length() > MAX_PASS_LEN)
		return false;
	for (std::size_t i = 0; i < password.length(); i++)
	{
		if (std::isspace(static_cast<unsigned char>(password[i])) ||
			!std::isprint(static_cast<unsigned char>(password[i])))
			return false;
	}

	return true;
}

bool isValidNick(const std::string &val)
{
	if (val.empty() || val.size() > MAX_NICK_LEN)
		return false;

	char c0 = val[0];
	bool firstOk = (c0 >= 'a' && c0 <= 'z') || (c0 >= 'A' && c0 <= 'Z') ||
				   c0 == '[' || c0 == ']' || c0 == '{' || c0 == '}' ||
				   c0 == '\\' || c0 == '|' || c0 == '_';
	if (!firstOk)
		return false;

	for (size_t i = 1; i < val.size(); i++)
	{
		if (!(std::isalnum(static_cast<unsigned char>(val[i])) ||
			  val[i] == '-' || val[i] == '_' || val[i] == '[' ||
			  val[i] == ']' || val[i] == '{' || val[i] == '}' ||
			  val[i] == '\\' || val[i] == '|'))
			return false;
	}

	return true;
}

bool isValidChannelName(const std::string &name)
{
	if (name.size() < 2 || name.size() > MAX_CHANNELNAME_LEN || name[0] != '#')
		return false;

	for (std::size_t i = 1; i < name.size(); ++i)
	{
		if (name[i] == ' ' || name[i] == '\a' || name[i] == ',')
			return false;
	}
	return true;
}
