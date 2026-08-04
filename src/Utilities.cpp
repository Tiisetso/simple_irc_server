#include "Utilities.hpp"

#include <string>
#include <vector>

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
