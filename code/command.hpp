#pragma once

#include <string>
#include <vector>

class server;

class Command {
protected:
    server* _server;
    
public:
    Command(server* srv);
    virtual ~Command();
    virtual void execute(int fd, const std::vector<std::string>& parsed) = 0;
};
