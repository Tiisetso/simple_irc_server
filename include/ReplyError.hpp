#pragma once

#include <string>
#include <unordered_map>

enum errReplyCode
{
	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHCHANNEL = 403,
	ERR_CANNOTSENDTOCHAN = 404,
	ERR_TOOMANYCHANNELS = 405,
	ERR_TOOMANYTARGETS = 407,
	ERR_NOORIGIN = 409,
	ERR_NORECIPIENT = 411,
	ERR_NOTEXTTOSEND = 412,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_ERRONEUSNICKNAME = 432,
	ERR_NICKNAMEINUSE = 433,
	ERR_CHANNELISFULL = 471,
	ERR_INVITEONLYCHAN = 473,
	ERR_BADCHANNELKEY = 475,
	ERR_NOTONCHANNEL = 442,
	ERR_NOTREGISTERED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_PASSWDMISMATCH = 464,
	ERR_BADCHANMASK = 476
};

const std::unordered_map<int, std::string> errReplyMsg = {
	{ERR_NOSUCHNICK, "No such nick/channel"},
	{ERR_NOSUCHCHANNEL, "No such channel"},
	{ERR_CANNOTSENDTOCHAN, "Cannot send to channel"},
	{ERR_TOOMANYCHANNELS, "You have joined too many channels"},
	{ERR_TOOMANYTARGETS, "Too many targets"},
	{ERR_NOORIGIN, "No origin specified"},
	{ERR_NORECIPIENT, "No recipient given (PRIVMSG)"},
	{ERR_NOTEXTTOSEND, "No text to send"},
	{ERR_NONICKNAMEGIVEN, "No nickname given"},
	{ERR_ERRONEUSNICKNAME, "Erroneous nickname"},
	{ERR_NICKNAMEINUSE, "Nickname is already in use"},
	{ERR_CHANNELISFULL, "Cannot join channel (+l)"},
	{ERR_INVITEONLYCHAN, "Cannot join channel (+i)"},
	{ERR_BADCHANNELKEY, "Cannot join channel (+k)"},
	{ERR_NOTONCHANNEL, "You're not on that channel"},
	{ERR_NEEDMOREPARAMS, "Not enough parameters"},
	{ERR_ALREADYREGISTERED, "You may not reregister"},
	{ERR_PASSWDMISMATCH, "Password incorrect"},
	{ERR_NOTREGISTERED, "You have not registered"},
	{ERR_BADCHANMASK, "Bad Channel Mask"}};
