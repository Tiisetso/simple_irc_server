#include "Utilities.hpp"

#include <ctime>
#include <string>
#include <vector>

std::time_t getCurrentTime()
{
	std::time_t now = std::time(nullptr);
	if (now == static_cast<std::time_t>(-1))
		return 0;
	return now;
}

bool lowerCaseEqual(const std::string &a, const std::string &b)
{
	if (a.size() != b.size())
		return false;

	for (size_t i = 0; i < a.size(); i++)
	{
		char left = a[i];
		char right = b[i];

		if (left >= 'A' && left <= 'Z')
			left += 'a' - 'A';
		if (right >= 'A' && right <= 'Z')
			right += 'a' - 'A';

		if (left != right)
			return false;
	}

	return true;
}

std::vector<std::string> splitStrToVec(const std::string &s, char delimiter)
{
	std::vector<std::string> stringVec;
	std::size_t pos;
	std::size_t start;

	start = 0;
	pos = s.find(delimiter, start);
	while (pos != std::string::npos)
	{
		stringVec.push_back(s.substr(start, pos - start));
		start = pos + 1;
		pos = s.find(delimiter, start);
	}
	stringVec.push_back(s.substr(start));
	return stringVec;
}
