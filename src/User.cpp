#include "User.hpp"

#include <unistd.h>

User::User() : _fd(-1), _readBuffer() {}

User::User(int fd) : _fd(fd), _readBuffer() {}

User::~User()
{
	if (_fd != -1)
		close(_fd);
}

int User::getFd() const
{
	return (_fd);
}

void User::setFd(int fd)
{
	_fd = fd;
}

std::string &User::getReadBuffer()
{
	return (_readBuffer);
}