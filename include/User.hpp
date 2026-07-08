#pragma once

#include <string>

class User
{
	private:
		int _fd;
		std::string _readBuffer;

	public:
		User();
		User(int fd);
		User(const User &other);
		User &operator=(const User &other);
		~User();
};