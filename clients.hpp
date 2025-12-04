#pragma once

#include <string>

class channel;

class clients
{
private:
    int _fd;
    std::string _nickname;
    std::string _username;
    std::string _hostName;
    std::string _realName;
    std::string buffer;
    bool _authentificated;
    bool _registered;
    void checkRegistration();

public:
    clients(int fd);
    clients();
    ~clients();

    void setAuthentificated(bool auth);
    bool isAuthentificated() const;
    bool isRegistered() const;

    int getFd() const;
    void setNickname(const std::string& nick);
    void setRealName(const std::string& real);
    void setUsername(const std::string& user);
    void setHostname(const std::string& host);
    std::string getNickname() const;
};
