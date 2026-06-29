
#include "Port.hpp"


bool validPort(const std::string& portString)
{
	unsigned int port = 0;
	char c;

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
