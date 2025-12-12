#include "channel.hpp"

channel::channel(std::string name, clients *creator) : _creator(creator)
{
	_name = name;
	_operators.insert(creator->getFd());
	addMember(creator->getFd(), creator);
	_topic = "No topic is set";
	_inviteOnly = false;
	_passwordProtected = false;
	_reservedTopic = false;
	_limitEnabled = false;
	_userLimit = 0;
}

void channel::addMember(int clientFd, clients *client)
{
	_members[clientFd] = client;
}

clients *channel::getCreator() const
{
	return (_creator);
}

void channel::addInvite(int clientFd, clients *client)
{
	_invited[clientFd] = client;
}
void channel::addOperator(int clientFd)
{
	_operators.insert(clientFd);
}

void channel::removeOperator(int clientFd)
{
	_operators.erase(clientFd);
}

bool channel::isOperator(int clientFd) const
{
	return (_operators.find(clientFd) != _operators.end());
}

channel::channel(const channel &other)
{
	*this = other;
}

channel &channel::operator=(const channel &other)
{
	if (this != &other)
	{
		_name = other._name;
		_creator = other._creator;
		_operators = other._operators;
		_topic = other._topic;
		_members = other._members;
	}
	return (*this);
}

void channel::removeMember(int clientFd)
{
	_members.erase(clientFd);
}

bool channel::isMember(int clientFd) const
{
	return (_members.find(clientFd) != _members.end());
}

void channel::setTopic(const std::string &topic)
{
	_topic = topic;
}

void channel::setReservedTopic(bool reservedTopic)
{
	_reservedTopic = reservedTopic;
}

void channel::setUserLimit(int limit)
{
	_userLimit = limit;
}

void channel::setLimitEnabled(bool limitEnabled)
{
	_limitEnabled = limitEnabled;
}

void channel::setInviteOnly(bool inviteOnly)
{
	_inviteOnly = inviteOnly;
}

void channel::setPasswordProtected(bool passwordProtected)
{
	_passwordProtected = passwordProtected;
}

void channel::setPassword(const std::string &password)
{
	_password = password;
}

bool channel::limitReached() const
{
	if (!_limitEnabled)
		return false;
	return (_members.size() >= static_cast<size_t>(_userLimit));
}

void channel::broadcastMessage(int senderFd, const std::string &message)
{
	std::map<int, clients *>::iterator it;
	for (it = _members.begin(); it != _members.end(); ++it)
	{
		int memberFd = it->first;
		if (memberFd != senderFd)
			send(memberFd, message.c_str(), message.length(), 0);
	}
}

std::string channel::getTopic() const
{
	return (_topic);
}

std::string channel::getName() const
{
    return _name;
}

std::string channel::getPassword() const
{
    return _password;
}

std::map<int, clients *> channel::getMembers() const
{
	return (_members);
}

std::map<int, clients *> channel::getInvited() const
{
    return _invited;
}

bool channel::getLimitEnabled() const
{
    return _limitEnabled;
}

int channel::getUserLimit() const
{
    return _userLimit;
}

bool channel::getReservedTopic() const
{
    return _reservedTopic;
}

bool channel::getInviteOnly() const
{
    return _inviteOnly;
}

bool channel::getPasswordProtected() const
{
    return _passwordProtected;
}

channel::channel()
{
}

channel::~channel()
{
}