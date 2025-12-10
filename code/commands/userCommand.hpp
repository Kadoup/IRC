#pragma once

#include "../command.hpp"

class UserCommand : public Command {
public:
    UserCommand(server* srv);
    void execute(int fd, const std::vector<std::string>& parsed);
};
