#include "channel.hpp"

void channel::addMember(int clientFd, clients* client)
{
    _members[clientFd] = client;
}

clients *channel::getCreator() const
{
    return _creator;
}

channel::channel(std::string name, clients* creator) : _creator(creator)
{
    _name = name;
    _operator = creator;
    addMember(creator->getFd(), creator);
}

channel::channel(const channel& other)
{
    _name = other._name;
    _creator = other._creator;
    _operator = other._operator;
    _topic = other._topic;
    _members = other._members;
}

// Assignment operator
channel& channel::operator=(const channel& other)
{
    if (this != &other)
    {
        _name = other._name;
        _creator = other._creator;
        _operator = other._operator;
        _topic = other._topic;
        _members = other._members;
    }
    return *this;
}

channel::channel()
{
}

channel::~channel()
{
}