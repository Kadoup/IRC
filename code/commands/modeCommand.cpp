#include "modeCommand.hpp"
#include "../server.hpp"

ModeCommand::ModeCommand(server* srv) : Command(srv) {
	
}

void ModeCommand::modeAdd()
{
	if (parsed[2][1] == 'i') {
		chan->setInviteOnly(true);
		std::string response = userId + " MODE " + target + " +i\r\n";
		std::map<int, clients*> members = chan->getMembers();
		std::map<int, clients*>::iterator memberIt;
		for (memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
			int memberFd = memberIt->first;
			send(memberFd, response.c_str(), response.length(), 0);
		}
	}
	else if (parsed[2][1] == 'o') {
		chan->addOperator(fd);
		std::cout << "You are now channel operator for " << target << std::endl;
	}
	else if (parsed[2][1] == 't') {
		std::cout << "Topic mode set to true (topic only changed by operators)" << std::endl;
		chan->setReservedTopic(true);
		// chan->setTopic(chan->getTopic()); // No change, just a placeholder
	}
	else if (parsed[2][1] == 'k') {
		chan->setPasswordProtected(true);
		chan->setPassword(parsed[3]);
		std::cout << "Password protection enabled" << std::endl;
	}
	else if (parsed[2][1] == 'l') {
		chan->setLimitEnabled(true);
		int limit = std::atoi(parsed[3].c_str());
		chan->setUserLimit(limit);
		std::cout << "User limit mode enabled with limit " << limit << std::endl;
	}
	else {
		std::cout << "Unknown mode to add" << std::endl;
					}
}

bool ModeCommand::channelExists(int fd, const std::string& target) {
	channel* chan = _server->getChannel(target);
	if (!chan) {
		std::cout << "No such channel" << std::endl;
		return false;
	}
	if (!chan->isMember(fd)) {
		std::cout << "You are not a member of this channel" << std::endl;
		return false;
	}
	return true;
}

void ModeCommand::execute(int fd, const std::vector<std::string>& parsed) {
	if (parsed.size() != 2 && parsed.size() != 3 && parsed.size() != 4) {
		std::cout << "Wrong number of arguments" << std::endl;
		return;
	}
	std::string target = parsed[1];
	if (target.empty()) {
		std::cout << "No target specified" << std::endl;
		return;
	}
	if (parsed.size() == 2) {
		if (target[0] == '#' || target[0] == '&' || target[0] == '+' || target[0] == '!') {
			if (channelExists(fd, parsed[1])) {
				std::string _response = ":localhost 324 " + _server->getClient(fd).getNickname() + 
										" " + parsed[1] + " " + _modes + "\r\n";
				send(fd, _response.c_str(), _response.length(), 0);
			}
		}
		else
			std::cout << "User modes are not supported" << std::endl;
	}
	else {
		if (parsed[2][0] != '+' && parsed[2][0] != '-') {
			std::cout << "Invalid mode format" << std::endl;
			return;
		}
		std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
		if (target[0] == '#' || target[0] == '&' || target[0] == '+' || target[0] == '!') {
			if (channelExists(fd, parsed[1])) {
				channel* chan = _server->getChannel(target);
				if (parsed[2][0] == '+') {
					if (parsed[2][1] == 'i') {
						chan->setInviteOnly(true);
						std::string response = userId + " MODE " + target + " +i\r\n";
						std::map<int, clients*> members = chan->getMembers();
						std::map<int, clients*>::iterator memberIt;
						for (memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
							int memberFd = memberIt->first;
							send(memberFd, response.c_str(), response.length(), 0);
						}
					}
					else if (parsed[2][1] == 'o') {
						chan->addOperator(fd);
						std::cout << "You are now channel operator for " << target << std::endl;
					}
					else if (parsed[2][1] == 't') {
						std::cout << "Topic mode set to true (topic only changed by operators)" << std::endl;
						chan->setReservedTopic(true);
						// chan->setTopic(chan->getTopic()); // No change, just a placeholder
					}
					else if (parsed[2][1] == 'k') {
						chan->setPasswordProtected(true);
						chan->setPassword(parsed[3]);
						std::cout << "Password protection enabled" << std::endl;
					}
					else if (parsed[2][1] == 'l') {
						chan->setLimitEnabled(true);
						int limit = std::atoi(parsed[3].c_str());
						chan->setUserLimit(limit);
						std::cout << "User limit mode enabled with limit " << limit << std::endl;
					}
					else {
						std::cout << "Unknown mode to add" << std::endl;
					}
				}
				else if (parsed[2][0] == '-')
				{
					if (parsed[2][1] == 'i') {
						chan->setInviteOnly(false);
						std::string response = ":" + _server->getClient(fd).getNickname() + "!" +
											   _server->getClient(fd).getUsername() + "@localhost MODE " +
											   target + " -i\r\n";
						std::map<int, clients*> members = chan->getMembers();
						std::map<int, clients*>::iterator memberIt;
						for (memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
							int memberFd = memberIt->first;
							send(memberFd, response.c_str(), response.length(), 0);
						}
					}
					else if (parsed[2][1] == 'o') {
						chan->removeOperator(fd);
						std::cout << "You are no longer channel operator for " << target << std::endl;
					}
					else if (parsed[2][1] == 't') {
						std::cout << "Topic mode set to true (topic only changed by operators)" << std::endl;
						channel* chan = _server->getChannel(target);
						chan->setReservedTopic(false);
						// chan->setTopic(chan->getTopic()); // No change, just a placeholder
					}
					else if (parsed[2][1] == 'k') {
						chan->setPasswordProtected(false);
						std::cout << "Password protection removed (not implemented)" << std::endl;
						
					}
					else if (parsed[2][1] == 'l') {
						chan->setLimitEnabled(false);
						chan->setUserLimit(0);
						std::cout << "User limit mode disabled" << std::endl;
					}
					else {
						std::cout << "Unknown mode to remove" << std::endl;
					}
				}
			}
		} else {
			std::cout << "User modes are not supported" << std::endl;
			}
	}
}