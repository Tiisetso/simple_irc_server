#pragma once

#include <string>
#include <unordered_map>

enum errReplyCode
{
	ERR_NOORIGIN = 409,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_PASSWDMISMATCH = 464,
};

const std::unordered_map<int, std::string> errReplyMsg = {
	{ERR_NOORIGIN, "No origin specified"},
	{ERR_NONICKNAMEGIVEN, "No nickname given"},
	{ERR_ERRONEUSNICKNAME, "Erroneus nickname"},
	{ERR_NICKNAMEINUSE, "Nickname is already in use"},
	{ERR_NEEDMOREPARAMS, "Not enough parameters"},
	{ERR_ALREADYREGISTERED, "You may not reregister"},
	{ERR_PASSWDMISMATCH, "Password incorrect"},
};