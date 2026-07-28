#include "Server.hpp"

void Server::handleQuit(const command &cmd, User &client)
{
	std::string disconnectReason = "Client quit";
	if (!cmd.vals.empty() && !cmd.vals[0].empty())
		disconnectReason = cmd.vals[0];
	client.setShouldDisconnect("Quit: " + disconnectReason);
}
