#pragma once
#include "../command.hpp"

class InviteCommand : public Command {
public:
	InviteCommand(server* srv);
	void execute(int fd, const std::vector<std::string>& parsed);
};