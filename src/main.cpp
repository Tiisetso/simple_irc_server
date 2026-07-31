#include <exception>
#include <iostream>

#include "Server.hpp"
#include "Validation.hpp"

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
	if (!validPassword(av[2]))
	{
		std::cerr << "Invalid password.\n";
		return (EXIT_FAILURE);
	}

	std::string port = av[1];
	std::string password = av[2];
	try
	{
		Server server(port, password);
		server.createSocket();
		server.signalSetup();
		server.loop();
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return (1);
	}
	std::cout << "Server: Bye bye." << std::endl;
	return (EXIT_SUCCESS);
}
