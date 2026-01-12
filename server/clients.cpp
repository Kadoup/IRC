#include "clients.hpp"
#include "channel.hpp"
#include <sys/socket.h>

void clients::checkRegistration()
{
    std::cout << "Checking registration for client FD " << _fd << std::endl;
    if (_authentificated && !_nickname.empty() && !_username.empty())
    {
        _registered = true;
        std::string welcome = ":localhost 001 " + _nickname + " :Welcome to the IRCedric " + _nickname + "!~" + _username + "@" + _hostName + "\r\n";
        welcome += ":localhost 004 " + _nickname + " localhost ircd-1.0 iouO lvhopsmntikrRcaqOALQbze lvhopsmntikrRcaqOALQbze\r\n";
        send(_fd, welcome.c_str(), welcome.length(), 0);
    }
}

clients::clients(int fd)
{
    _fd = fd;
    _hostName = "localhost";
    _registered = false;
    _authentificated = false;
}

clients::clients()
{
    _fd = 0;
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
    if (_authentificated == true)
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

void clients::setRealName(const std::string &real)
{
    _realName = real;
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

std::string clients::getNickname() const
{
    return _nickname;
}

std::string clients::getBuffer() const
{
    return _buffer;
}

std::string clients::getHostname() const
{
    return _hostName;
}

void clients::clearBuffer(size_t n)
{
    if (n >= _buffer.size())
        _buffer.clear();
    else
        _buffer.erase(0, n + 2);
}

std::string clients::getUsername() const
{
    return _username;
}

std::string clients::getRealName() const
{
    return _realName;
}

void clients::setBuffer(const std::string &buf)
{
    _buffer += buf;
}



std::ostream& operator<<(std::ostream& os, const clients& client)
{
    os << "Client Info:" << std::endl;
    os << "  FD: " << client.getFd() << std::endl;
    os << "  Nickname: " << client.getNickname() << std::endl;
    os << "  Username: " << client.getUsername() << std::endl;
    os << "  Real Name: " << client.getRealName() << std::endl;
    os << "  Authenticated: " << (client.isAuthentificated() ? "Yes" : "No") << std::endl;
    os << "  Registered: " << (client.isRegistered() ? "Yes" : "No") << std::endl;
    return os;
}
