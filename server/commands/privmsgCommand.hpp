#pragma once

#include "../command.hpp"

class PrivmsgCommand : public Command {
public:
    PrivmsgCommand(server* srv);
    void execute(int fd, const std::vector<std::string>& parsed);
};
