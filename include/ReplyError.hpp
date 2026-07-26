#pragma once

#include <string>
#include <unordered_map>

enum errReplyCode
{
	ERR_NOSUCHCHANNEL = 403,
	ERR_NOORIGIN = 409,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_NOTONCHANNEL = 442,
	ERR_NOTREGISTERED = 451, 
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_PASSWDMISMATCH = 464,
};

const std::unordered_map<int, std::string> errReplyMsg = {
	{ERR_NOSUCHCHANNEL, "No such channel"},
	{ERR_NOORIGIN, "No origin specified"},
	{ERR_NONICKNAMEGIVEN, "No nickname given"},
	{ERR_ERRONEUSNICKNAME, "Erroneous nickname"},
	{ERR_NICKNAMEINUSE, "Nickname is already in use"},
	{ERR_NOTONCHANNEL, "You're not on that channel"},
	{ERR_NEEDMOREPARAMS, "Not enough parameters"},
	{ERR_ALREADYREGISTERED, "You may not reregister"},
	{ERR_PASSWDMISMATCH, "Password incorrect"},
	{ERR_NOTREGISTERED, "You have not registered"},
};