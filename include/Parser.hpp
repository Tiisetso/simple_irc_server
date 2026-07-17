#pragma once

#include <string>
#include <vector>

struct command
{
		std::string key;
		std::vector<std::string> vals;
};

bool parseCommand(const std::string &msg, command &cmd);
