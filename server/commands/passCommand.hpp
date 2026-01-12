#pragma once

#include "../command.hpp"

class PassCommand : public Command {
public:
    PassCommand(server* srv);
    void execute(int fd, const std::vector<std::string>& parsed);
};
