#pragma once

#include "../command.hpp"

class JoinCommand : public Command {
public:
    JoinCommand(server* srv);
    void execute(int fd, const std::vector<std::string>& parsed);
};
