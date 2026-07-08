#include "Server.hpp"

User::User() : _fd(-1), _readBuffer() {}

User::User(int fd) : _fd(fd), _readBuffer() {}

User::User(const User &other) : _fd(other._fd), _readBuffer(other._readBuffer)
{
}

User &User::operator=(const User &other)
{
	if (this != &other)
	{
		_fd = other._fd;
		_readBuffer = other._readBuffer;
	}
	return (*this);
}

User::~User()
{
	if (_fd != -1)
		close(_fd);
}
