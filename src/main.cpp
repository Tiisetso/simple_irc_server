#include "Server.hpp"
#include <iostream>

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: ./ircserv <port>" << std::endl;
		return 1;
	}

	std::string port = av[1];

	Server server(port);
	if (server.createSocket())
		return 1;

	return 0;
}
