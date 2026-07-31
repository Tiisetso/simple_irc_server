#include "Server.hpp"

void Server::handleCap(const command &cmd, User &client)
{
	if (client.getIsRegistered())
		return;

	if (!cmd.vals.empty() && cmd.vals[0] == "LS")
	{
		client.setCapInProgress(true);
		queueMessage(client, msgCap(client));
		return;
	}
	if (!cmd.vals.empty() && cmd.vals[0] == "END")
	{
		client.setCapInProgress(false);
		registerClient(client);
		return;
	}
}