#include "Channel.hpp"

Channel::~Channel() {}

Channel::Channel()
	: _name(""),
	  _topic(""),
	  _topicSetBy(""),
	  _topicSetAt(0),
	  _key(""),
	  _limit(0),
	  _inviteOnly(false),
	  _topicRestricted(false),
	  _hasKey(false),
	  _hasLimit(false)
{
}

Channel::Channel(const std::string &name)
	: _name(name),
	  _topic(""),
	  _topicSetBy(""),
	  _topicSetAt(0),
	  _key(""),
	  _limit(0),
	  _inviteOnly(false),
	  _topicRestricted(false),
	  _hasKey(false),
	  _hasLimit(false)
{
}

const std::string &Channel::getName() const
{
	return _name;
}

void Channel::setName(const std::string &name)
{
	_name = name;
}

const std::string &Channel::getTopic() const
{
	return _topic;
}

const std::string &Channel::getTopicSetBy() const
{
	return _topicSetBy;
}

std::time_t Channel::getTopicSetAt() const
{
	return _topicSetAt;
}

void Channel::setTopic(const std::string &topic, const std::string &setBy,
					   std::time_t setAt)
{
	_topic = topic;
	if (topic.empty())
	{
		_topicSetBy.clear();
		_topicSetAt = 0;
	}
	else
	{
		_topicSetBy = setBy;
		_topicSetAt = setAt;
	}
}

bool Channel::isTopicRestricted() const
{
	return _topicRestricted;
}

void Channel::setTopicRestricted(bool flag)
{
	_topicRestricted = flag;
}

void Channel::setKey(const std::string &key)
{
	_key = key;
	_hasKey = true;
}

void Channel::removeKey()
{
	_key = "";
	_hasKey = false;
}

const std::string &Channel::getKey() const
{
	return _key;
}

bool Channel::hasKey() const
{
	return _hasKey;
}

bool Channel::isInviteOnly() const
{
	return _inviteOnly;
}

void Channel::setInviteOnly(bool flag)
{
	_inviteOnly = flag;
}

void Channel::inviteUser(User &user)
{
	_invitedUsers.insert(&user);
}

bool Channel::isUserInvited(User &user) const
{
	if (_invitedUsers.count(&user) == 0)
		return false;
	return true;
}

void Channel::setLimit(std::size_t limitMax)
{
	_limit = limitMax;
	_hasLimit = true;
}

void Channel::removeLimit()
{
	_hasLimit = false;
	_limit = 0;
}

bool Channel::isLimitReached() const
{
	if (_hasLimit)
		return (_users.size() >= _limit);
	return false;
}

void Channel::addUser(User &user)
{
	_users.insert(&user);
	_invitedUsers.erase(&user);
}

void Channel::removeUser(User &user)
{
	_users.erase(&user);
	_operators.erase(&user);
	_invitedUsers.erase(&user);
}

bool Channel::isUserInChannel(User &user) const
{
	if (_users.count(&user) == 0)
		return false;
	return true;
}

const std::set<User *> &Channel::getUsers() const
{
	return _users;
}

void Channel::addOperator(User &user)
{
	if (isUserInChannel(user))
		_operators.insert(&user);
}

void Channel::removeOperator(User &user)
{
	_operators.erase(&user);
}

bool Channel::isOperator(User &user) const
{
	if (_operators.count(&user) == 0)
		return false;
	return true;
}

std::string Channel::getMode(bool isMember) const
{
	std::string modeString{};
	std::string argument{};

	if (_inviteOnly)
		modeString += 'i';
	if (_topicRestricted)
		modeString += 't';
	if (_hasKey)
	{
		modeString += 'k';
		if (isMember)
			argument += " " + _key;
	}
	if (_hasLimit)
	{
		modeString += 'l';
		argument += " " + std::to_string(_limit);
	}

	return "+" + modeString + argument;
}
