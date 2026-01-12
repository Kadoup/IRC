#pragma once

#include "../command.hpp"

class WhoCommand : public Command {
public:
    WhoCommand(server* srv);
    void execute(int fd, const std::vector<std::string>& parsed);
};