#include "Port.hpp"
#include "Server.hpp"
#include <exception>
#include <iostream>

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password> \n";
		return (EXIT_FAILURE);
	}
	if (!validPort(av[1]))
	{
		std::cerr << "Invalid port.\n";
		return (EXIT_FAILURE);
	}

	std::string port = av[1];
	try
	{
		Server server(port);
		server.createSocket();
		server.acceptClient();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (EXIT_SUCCESS);
}
