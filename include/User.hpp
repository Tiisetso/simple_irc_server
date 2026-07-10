#pragma once

#include <string>

class User
{
	private:
		int _fd;
		std::string _readBuffer;

	public:
		User();
		~User();
		User(int fd);
		User(const User &other) = delete;
		User &operator=(const User &other) = delete;

		int getfd() const;
		void setfd(int fd);
		std::string &getReadBuffer();
};