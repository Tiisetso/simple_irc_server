#pragma once

#include <string>

class User
{
	private:
		int _fd;
		bool _passMatch;
		bool _isRegistered;
		bool _shouldDisconnect;
		bool _capInProgress;
		std::string _readBuffer;
		std::string _writeBuffer;
		std::string _nickName;
		std::string _userName;
		std::string _realName;
		std::string _host;

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
		bool getIsRegistered() const;
		bool getShouldDisconnect() const;
		bool getCapInProgress() const;
		const std::string &getNickName() const;
		const std::string &getUserName() const;
		const std::string &getRealName() const;
		const std::string &getHost() const;

		void setFd(const int fd);
		void setPassMatch(const bool yes);
		void setIsRegistered(void);
		void setShouldDisconnect(void);
		void setCapInProgress(bool value);
		void setNickName(const std::string &nickName);
		void setUserName(const std::string &userName);
		void setRealName(const std::string &realName);
		void setHost(const std::string &host);
};