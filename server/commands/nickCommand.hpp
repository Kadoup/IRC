#pragma once

#include "../command.hpp"

class NickCommand : public Command {
public:
    NickCommand(server* srv);
    void execute(int fd, const std::vector<std::string>& parsed);
};
