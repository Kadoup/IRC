#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "clients.hpp"

class channel
{
private:
    std::map<int, clients*> _members;
    std::set<int> _operators;
    std::string _name;
    clients* _creator;
    std::string _topic;
    
    public:
    void addMember(int clientFd, clients* client);
    clients* getCreator() const;
    std::string getTopic() const;
    bool isOperator(int clientFd) const;
    void addOperator(int clientFd);
    void removeOperator(int clientFd);
    void setTopic(const std::string& topic);
    channel& operator=(const channel& other);
    channel(std::string name, clients* creator);
    channel(const channel& other);
    channel();
    ~channel();
};
