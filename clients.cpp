#include "clients.hpp"

void clients::checkRegistration()
{
    if (_authetificated && !_nickname.empty() && _username.empty())
        _registered = true;
}

clients::clients(int fd)
{
    _fd = fd;
    _registered = false;
    _authentificated = false;
}

clients::~clients()
{
}

void clients::setAuthentificated(bool auth)
{
    _authentificated = auth;
}

bool clients::isAuthentificated() const
{
    if (_autentificated == true)
        return true;
    return false;
}

bool clients::isRegistered() const
{
    if (_registered == true)
        return true;
    return false;
}

int clients::getFd() const
{
    return _fd;
}

void clients::setNickname(const std::string &nick)
{
    _nickname = nick;
    checkRegistration();
}

void clients::setUsername(const std::string &user)
{
    _username = user;
    checkRegistration();
}

void clients::setHostname(const std::string &host)
{
    _hostName = host;
}
