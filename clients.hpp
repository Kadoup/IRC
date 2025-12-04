#include "channel.hpp"

class clients
{
private:
    int _fd;
    std::string _nickname;
    std::string _username;
    std::string _hostName;
    std::string buffer;
    bool _authentifcated;
    bool _registered;
    void checkRegistration();

public:
    clients(int fd);
    ~clients();

    void setAuthentificated(bool auth);
    bool isAuthentificated() const;
    bool isRegistered() const;

    int getFd() const;
    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void setHostname(const std::string& host);
};
