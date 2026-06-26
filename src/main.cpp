
#include <iostream>
#include "parse_port.hpp"

int main(int ac, char **av)
{
	if (ac != 3)
	{
		std::cerr << "Usage: ./ircserv <port> <password> \n";
		return (EXIT_FAILURE);
	}
	if(!parsePort(av[1]))
	{
		std::cerr << "Invalid port.\n";
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
