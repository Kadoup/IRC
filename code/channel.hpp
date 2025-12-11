#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>
#include <sys/socket.h>

#include "clients.hpp"

class channel
{
private:
    std::map<int, clients*> _members;
    std::set<int> _operators;
    std::string _name;
    clients* _creator;
    std::string _topic;
    bool _inviteOnly;
    bool _passwordProtected;
    std::map<int, clients*> _invited;
    
    public:
    void addMember(int clientFd, clients* client);
    void addInvite(int clientFd, clients *client);
    void removeMember(int clientFd);
    bool isMember(int clientFd) const;
    clients* getCreator() const;
    std::string getTopic() const;
    std::string getName() const;
    std::map<int, clients*> getMembers() const;
    std::map<int, clients*> getInvited() const;
    bool getInviteOnly() const;
    bool getPasswordProtected() const;
    bool isOperator(int clientFd) const;
    void addOperator(int clientFd);
    void removeOperator(int clientFd);
    void setTopic(const std::string& topic);
	void setInviteOnly(bool inviteOnly);
    void setPasswordProtected(bool passwordProtected);
    void broadcastMessage(int senderFd, const std::string& message);
    channel& operator=(const channel& other);
    channel(std::string name, clients* creator);
    channel(const channel& other);
    channel();
    ~channel();
};
