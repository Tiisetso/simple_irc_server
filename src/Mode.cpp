#include "ReplyError.hpp"
#include "Server.hpp"
#include <iostream>

void Server::handleUserMode(const command &cmd, User &client)
{
    (void)cmd;
    (void)client;
}

void Server::handleChannelMode(const command &cmd, User &client, const std::string &target)
{
    Channel *channel = getChannel(target);

    if (!channel)
    {
        queueMessage(client, msgReply(client, ERR_NOSUCHCHANNEL, target));
        return;
    }
    if (cmd.vals.size() == 1)
    {
        // TODO: if (cmd.vals.size() == 1) reply with the channel's current modes if channel mode is given.
        //queueMessage(client, queueMessage(client, msgNumeric(client, int code, const std::string &middle, const std::string &trailing)))
        //324 329
        return;
    }
    if (!channel->isOperator(client))
    {
        queueMessage(client, msgReply(client, ERR_CHANOPRIVSNEEDED, target));
        return;
    }
}

void Server::handleMode(const command &cmd, User &client)
{
    std::cout << "mode handler reached" << std::endl;
    if (cmd.vals.empty())
    {
        queueMessage(client, msgReply(client, ERR_NEEDMOREPARAMS));
        return;
    }

    const std::string &target = cmd.vals[0];

    if (target[0] != '#')
    {
        handleUserMode(cmd, client);
        return;
    }

    handleChannelMode(cmd, client, target);
}
