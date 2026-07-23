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
		// TODO: operators

	public:
		Channel() = delete;
		Channel(const std::string &name);
		~Channel();

		const std::string &getName() const;

		const std::string &getTopic() const;
		void setTopic(const std::string &topic);
		bool isTopicRestricted() const;
		void setTopicRestricted(bool flag);

		void setKey(const std::string &key);
		void removeKey();
		std::string getKey() const;
		bool hasKey() const;

		bool isInviteOnly() const;
		void setInviteOnly(bool flag);
		void inviteUser(User &user);
		bool isUserInvited(User &user) const;


		void setLimit(std::size_t limitMax);
		void removeLimit();
		bool isLimitReached() const;

		void addUser(User &user);
		void removeUser(User &user);
		bool isUserInChannel(User &user) const;
		const std::set<User *> &getUsers() const;

		// TODO: operators
};
