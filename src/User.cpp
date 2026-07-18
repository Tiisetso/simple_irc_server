#include "User.hpp"

#include <unistd.h>

User::User()
	: _fd(-1),
	  _passMatch(false),
	  _isRegistered(false),
	  _readBuffer(),
	  _nickName(),
	  _userName(),
	  _realName()
{
}

User::User(int fd)
	: _fd(fd),
	  _passMatch(false),
	  _isRegistered(false),
	  _readBuffer(),
	  _nickName(),
	  _userName(),
	  _realName()
{
}

User::~User()
{
	if (_fd != -1)
		close(_fd);
}

int User::getFd() const
{
	return (_fd);
}

std::string &User::getReadBuffer()
{
	return (_readBuffer);
}

bool User::getPassMatch() const
{
	return _passMatch;
}

bool User::getIsRegistered() const
{
	return _isRegistered;
}

const std::string &User::getNickName() const
{
	return _nickName;
}

const std::string &User::getUserName() const
{
	return _userName;
}

const std::string &User::getRealName() const
{
	return _realName;
}

void User::setFd(const int fd)
{
	_fd = fd;
}

void User::setPassMatch(const bool yes)
{
	_passMatch = yes;
}

void User::setIsRegistered(void)
{
	_isRegistered = true;
}

void User::setNickName(const std::string &nickName)
{
	_nickName = nickName;
}

void User::setUserName(const std::string &userName)
{
	_userName = userName;
}

void User::setRealName(const std::string &realName)
{
	_realName = realName;
}
