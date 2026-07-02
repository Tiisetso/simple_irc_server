#include "Port.hpp"

#include <string>

bool validPort(const std::string &portString)
{
	unsigned int port;
	char c;

	port = 0;
	if (portString.empty() || portString.length() > 5)
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
