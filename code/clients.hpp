#pragma once

#include <string>
#include <iostream>

class channel;

class clients
{
private:
    int _fd;
    std::string _nickname;
    std::string _username;
    std::string _hostName;
    std::string _realName;
    std::string _buffer;
    bool _authentificated;
    bool _registered;
    
    public:
    clients(int fd);
    clients();
    ~clients();
    
    void checkRegistration();
    void setAuthentificated(bool auth);
    bool isAuthentificated() const;
    bool isRegistered() const;
    std::string getBuffer() const;
    void clearBuffer(size_t n);
    int getFd() const;
    void setNickname(const std::string& nick);
    void setRealName(const std::string& real);
    void setUsername(const std::string& user);
    void setHostname(const std::string& host);
    void setBuffer(const std::string& buf);
    std::string getNickname() const;
    std::string getUsername() const;
    std::string getRealName() const;
    std::string getHostname() const;
};
std::ostream& operator<<(std::ostream& os, const clients& client);
