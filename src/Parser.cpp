#include "Parser.hpp"

#include <cctype>

static std::size_t afterSpaces(const std::string &s, std::size_t start,
							   std::size_t end)
{
	while (start < end && s[start] == ' ')
		start++;

	return start;
}

static std::size_t nextSpace(const std::string &s, std::size_t start,
							 std::size_t end)
{
	std::size_t spacePosition{};

	spacePosition = s.find(' ', start);
	if (spacePosition == std::string::npos)
		return end;

	return spacePosition;
}

bool parseCommand(const std::string &msg, command &cmd)
{
	std::size_t end{};
	std::size_t start{};
	std::size_t tokenEnd{};

	end = msg.length();

	while (end > 0 && (msg[end - 1] == '\r' || msg[end - 1] == '\n'))
		end--;

	if (end == 0)
		return false;

	start = afterSpaces(msg, start, end);

	if (start >= end)
		return false;

	tokenEnd = nextSpace(msg, start, end);
	cmd.key = msg.substr(start, tokenEnd - start);
	for (std::size_t i = 0; i < cmd.key.length(); i++)
		cmd.key[i] = static_cast<char>(
			std::toupper(static_cast<unsigned char>(cmd.key[i])));
	start = tokenEnd;

	while (true)
	{
		start = afterSpaces(msg, start, end);
		if (start >= end)
			break;

		if (msg[start] == ':')
		{
			cmd.vals.push_back(msg.substr(start + 1, end - start - 1));
			break;
		}

		std::size_t val = nextSpace(msg, start, end);
		cmd.vals.push_back(msg.substr(start, val - start));
		start = val;
	}
	return true;
}
