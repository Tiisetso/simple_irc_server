#pragma once

#include <string>

class User
{
	private:
		int _fd;
		bool _passMatch;
		std::string _readBuffer;
		std::string _writeBuffer;
		std::string _nickName;
		std::string _userName;
		std::string _realName;

	public:
		User();
		~User();
		User(int fd);
		User(const User &other) = delete;
		User &operator=(const User &other) = delete;

		int getFd() const;
		std::string &getReadBuffer();
		std::string &getWriteBuffer();
		bool getPassMatch() const;
		const std::string &getNickName() const;
		const std::string &getUserName() const;
		const std::string &getRealName() const;

		void setFd(const int fd);
		void setPassMatch(const bool yes);
		void setNickName(const std::string &nickName);
		void setUserName(const std::string &userName);
		void setRealName(const std::string &realName);
};