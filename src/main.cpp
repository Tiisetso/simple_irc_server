#include <exception>
#include <iostream>
#include "Port.hpp"
#include "Server.hpp"

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password> \n";
		return (EXIT_FAILURE);
	}
	if(!validPort(av[1]))
	{
		std::cerr << "Invalid port.\n";
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);

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
