#pragma once

#include "../command.hpp"

class QuitCommand : public Command {
public:
    QuitCommand(server* srv);
    void execute(int fd, const std::vector<std::string>& parsed);
};
