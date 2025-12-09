#include "channel.hpp"

void channel::addMember(int clientFd, clients* client)
{
    _members[clientFd] = client;
}

clients *channel::getCreator() const
{
    return _creator;
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
    return _operators.find(clientFd) != _operators.end();
}

channel::channel(std::string name, clients* creator) : _creator(creator)
{
    _name = name;
    _operators.insert(creator->getFd());
    addMember(creator->getFd(), creator);
}

channel::channel(const channel& other)
{
    *this = other;
}

channel& channel::operator=(const channel& other)
{
    if (this != &other)
    {
        _name = other._name;
        _creator = other._creator;
        _operators = other._operators;
        _topic = other._topic;
        _members = other._members;
    }
    return *this;
}

void channel::setTopic(const std::string& topic)
{
    _topic = topic;
}

std::string channel::getTopic() const
{
    return _topic;
}

channel::channel()
{
    _topic = "No topic is set";
}

channel::~channel()
{
}