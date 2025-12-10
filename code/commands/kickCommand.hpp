#pragma once

#include "../command.hpp"

class KickCommand : public Command {
public:
    KickCommand(server* srv);
    void execute(int fd, const std::vector<std::string>& parsed);
};
