#include "clients.hpp"

clients::clients(int fd)
{
    _fd = fd;
}

clients::~clients()
{
}

void clients::getFd() const
{
    return _fd;
}

void clients::setNickname(const std::string &nick)
{
    _nickname = nick;
}

void clients::setUsername(const std::string &user)
{
    _username = user;
}

void clients::setRealname(const std::string &real)
{
    _realName = real;
}

void clients::setHostname(const std::string &host)
{
    _hostName = host;
}
