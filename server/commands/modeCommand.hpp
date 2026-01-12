#pragma once
#include "../command.hpp"

class ModeCommand : public Command {
	private:
		std::string _modes;
		bool channelExists(int fd, const std::string& target);
	public:
		ModeCommand(server* srv);
		void execute(int fd, const std::vector<std::string>& parsed);
};