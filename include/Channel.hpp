#pragma once

#include <set>
#include <string>

#include "User.hpp"

class Channel
{
	private:
		std::string _name;
		std::string _topic;
		std::string _key;
		std::size_t _limit;

		bool _inviteOnly;
		bool _topicRestricted;
		bool _hasKey;
		bool _hasLimit;

		std::set<User *> _users;
		std::set<User *> _invitedUsers;
		std::set<User *> _operators;

	public:
		~Channel();
		Channel();
		Channel(const std::string &name);

		const std::string &getName() const;
		void setName(const std::string &name);

		const std::string &getTopic() const;
		void setTopic(const std::string &topic);
		bool isTopicRestricted() const;
		void setTopicRestricted(bool flag);

		void setKey(const std::string &key);
		void removeKey();
		const std::string &getKey() const;
		bool hasKey() const;

		bool isInviteOnly() const;
		void setInviteOnly(bool flag);
		void inviteUser(User &user);
		bool isUserInvited(User &user) const;

		void setLimit(std::size_t limitMax);
		bool isLimitReached() const;
		void removeLimit();

		void addUser(User &user);
		void removeUser(User &user);
		bool isUserInChannel(User &user) const;
		const std::set<User *> &getUsers() const;

		void addOperator(User &user);
		void removeOperator(User &user);
		bool isOperator(User &user) const;

		std::string getMode(bool isMember) const;
};
