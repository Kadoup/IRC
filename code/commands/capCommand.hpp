#pragma once
#include "../command.hpp"

class CapCommand : public Command {
public:
	CapCommand(server* srv);
	void execute(int fd, const std::vector<std::string>& parsed);
};