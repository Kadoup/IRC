#pragma once

#include <string>
#include <vector>
#include <map>

#include "clients.hpp"

class channel
{
private:
    std::map<int, clients*> _members;
    std::string _name;
    clients* _creator;
    clients* _operator;
    std::string _topic;
    
    public:
    void addMember(int clientFd, clients* client);
    clients* getCreator() const;
    channel(std::string name, clients* creator);
    channel(const channel& other);  // Copy constructor
    channel& operator=(const channel& other);
    channel();
    ~channel();
};
