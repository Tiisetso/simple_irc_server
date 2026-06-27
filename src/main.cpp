#include "Server.hpp"
#include <exception>
#include <iostream>

int main(int ac, char **av)
{
	if (ac != 2)
	{
		std::cerr << "Usage: ./ircserv <port>" << std::endl;
		return 1;
	}

	std::string port = av[1];

	try
	{
		Server server(port);
		server.createSocket();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}


	return 0;
}
