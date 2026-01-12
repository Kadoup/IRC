#pragma once

#include "../command.hpp"
#include "../channel.hpp"

class JoinCommand : public Command {
    private:
    bool isInvited(int fd, channel& chan);
public:
    JoinCommand(server* srv);
    void execute(int fd, const std::vector<std::string>& parsed);
};
