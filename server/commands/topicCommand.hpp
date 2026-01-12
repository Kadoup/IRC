#pragma once

#include "../command.hpp"

class TopicCommand : public Command {
public:
    TopicCommand(server* srv);
    void execute(int fd, const std::vector<std::string>& parsed);
};
