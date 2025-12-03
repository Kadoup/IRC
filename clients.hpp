#include "channel.hpp"

class clients
{
private:
    int _fd;
    std::string _nickname;
    std::string _username;
    std::string _realName;
    std::string _hostName;
    std::string buffer;
    bool _authentifcated;

public:
    clients(int fd);
    ~clients();

    void getFd() const;
    void setNickname(const std::string& nick);
    void setUsername(const std::string& user);
    void setRealname(const std::string& real);
    void setHostname(const std::string& host);
};
