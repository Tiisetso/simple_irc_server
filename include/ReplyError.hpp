#pragma once

#include <string>
#include <unordered_map>

enum errReplyCode
{
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_PASSWDMISMATCH = 464,
};

const std::unordered_map<int, std::string> errReplyMsg = {
	{ERR_NEEDMOREPARAMS, "Not enough parameters"},
	{ERR_ALREADYREGISTERED, "You may not reregister"},
	{ERR_PASSWDMISMATCH, "Password incorrect"},
};