#include "modeCommand.hpp"
#include "../server.hpp"

ModeCommand::ModeCommand(server* srv) : Command(srv) {
	
}

// void ModeCommand::modeAdd()
// {
// 	if (parsed[2][1] == 'i') {
// 		chan->setInviteOnly(true);
// 		std::string response = userId + " MODE " + target + " +i\r\n";
// 		std::map<int, clients*> members = chan->getMembers();
// 		std::map<int, clients*>::iterator memberIt;
// 		for (memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
// 			int memberFd = memberIt->first;
// 			send(memberFd, response.c_str(), response.length(), 0);
// 		}
// 	}
// 	else if (parsed[2][1] == 'o') {
// 		chan->addOperator(fd);
// 		std::cout << "You are now channel operator for " << target << std::endl;
// 	}
// 	else if (parsed[2][1] == 't') {
// 		std::cout << "Topic mode set to true (topic only changed by operators)" << std::endl;
// 		chan->setReservedTopic(true);
// 		// chan->setTopic(chan->getTopic()); // No change, just a placeholder
// 	}
// 	else if (parsed[2][1] == 'k') {
// 		chan->setPasswordProtected(true);
// 		chan->setPassword(parsed[3]);
// 		std::cout << "Password protection enabled" << std::endl;
// 	}
// 	else if (parsed[2][1] == 'l') {
// 		chan->setLimitEnabled(true);
// 		int limit = std::atoi(parsed[3].c_str());
// 		chan->setUserLimit(limit);
// 		std::cout << "User limit mode enabled with limit " << limit << std::endl;
// 	}
// 	else {
// 		std::cout << "Unknown mode to add" << std::endl;
// 					}
// }

bool ModeCommand::channelExists(int fd, const std::string& target) {
	channel* chan = _server->getChannel(target);
	std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
	if (!chan) {
		std::string response = ERR_NOSUCHCHANNEL(userId, _server->getClient(fd).getNickname(), target);
		send(fd, response.c_str(), response.length(), 0);
		return false;
	}
	if (!chan->isMember(fd)) {
		std::string response = ERR_NOTONCHANNEL(userId, _server->getClient(fd).getNickname(), target);
		send(fd, response.c_str(), response.length(), 0);
		return false;
	}
	return true;
}

void ModeCommand::execute(int fd, const std::vector<std::string>& parsed) {
	std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
	if (parsed.size() != 2 && parsed.size() != 3 && parsed.size() != 4) {
		std::string response = ERR_NEEDMOREPARAMS(userId, _server->getClient(fd).getNickname(), "MODE");
		send(fd, response.c_str(), response.length(), 0);
		return;
	}
	std::string target = parsed[1];
	if (target.empty()) {
		std::string response = ERR_NEEDMOREPARAMS(userId, _server->getClient(fd).getNickname(), "MODE");
		return;
	}
	if (parsed.size() == 2) {
		if (target[0] == '#' || target[0] == '&' || target[0] == '+' || target[0] == '!') {
			if (channelExists(fd, parsed[1])) {
				std::string response = RPL_CHANNELMODEIS(userId, _server->getClient(fd).getNickname(), target, _modes);
				send(fd, response.c_str(), response.length(), 0);
			}
		}
		else
		{
			std::string response = ERR_UMODEUNKNOWNFLAG(userId, _server->getClient(fd).getNickname());
			send(fd, response.c_str(), response.length(), 0);
		}
	}
	else {
		if (parsed[2][0] != '+' && parsed[2][0] != '-') {
			std::string response = ERR_UNKNOWNMODE(userId, _server->getClient(fd).getNickname(), parsed[2]);
			send(fd, response.c_str(), response.length(), 0);
			return;
		}
		if (target[0] == '#' || target[0] == '&' || target[0] == '+' || target[0] == '!') {
			if (channelExists(fd, parsed[1])) {
				channel* chan = _server->getChannel(target);
				if (parsed[2][0] == '+') {
					if (parsed[2][1] == 'i') {
						chan->setInviteOnly(true);
						std::string response = userId + " MODE " + target + " +i\r\n";
						send (fd, response.c_str(), response.length(), 0);
						std::map<int, clients*> members = chan->getMembers();
						std::map<int, clients*>::iterator memberIt;
						for (memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
							int memberFd = memberIt->first;
							send(memberFd, response.c_str(), response.length(), 0);
						}
					}
					else if (parsed[2][1] == 'o') {
						int targetFd = _server->findClientByNickname(parsed[3]);
						if (targetFd == -1) {
							std::string response = ERR_NOSUCHNICK(userId, _server->getClient(fd).getNickname(), parsed[3]);
							send(fd, response.c_str(), response.length(), 0);
							return;
						}
						if (!chan->isMember(targetFd)) {
							std::string response = ERR_USERNOTINCHANNEL(userId, _server->getClient(fd).getNickname(), target, parsed[3]);
							send (fd, response.c_str(), response.length(), 0);
							return;
						}
						chan->addOperator(targetFd);
						std::string response = userId + " MODE " + target + " +o " + parsed[3] + "\r\n";
						std::map<int, clients*> members = chan->getMembers();
						for (std::map<int, clients*>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
							int memberFd = memberIt->first;
							send(memberFd, response.c_str(), response.length(), 0);
						}
						// send (targetFd, response.c_str(), response.length(), 0);
					}
					else if (parsed[2][1] == 't') {
						std::string response = userId + " MODE " + target + " +t\r\n";
						send (fd, response.c_str(), response.length(), 0);
						chan->setReservedTopic(true);
						// chan->setTopic(chan->getTopic()); // No change, just a placeholder
					}
					else if (parsed[2][1] == 'k') {
						chan->setPasswordProtected(true);
						chan->setPassword(parsed[3]);
						std::string response = userId + " MODE " + target + " +k " + parsed[3] + "\r\n";
						send (fd, response.c_str(), response.length(), 0);
					}
					else if (parsed[2][1] == 'l') {
						chan->setLimitEnabled(true);
						int limit = std::atoi(parsed[3].c_str());
						chan->setUserLimit(limit);
						std::string response = userId + " MODE " + target + " +l " + parsed[3] + "\r\n";
						send (fd, response.c_str(), response.length(), 0);
					}
					else {
						std::string response = ERR_UNKNOWNMODE(userId, _server->getClient(fd).getNickname(), parsed[2]);
						send(fd, response.c_str(), response.length(), 0);
					}
				}
				else if (parsed[2][0] == '-')
				{
					if (parsed[2][1] == 'i') {
						chan->setInviteOnly(false);
						std::string response = userId + " MODE " + target + " -i\r\n";
						send (fd, response.c_str(), response.length(), 0);
						std::map<int, clients*> members = chan->getMembers();
						std::map<int, clients*>::iterator memberIt;
						for (memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
							int memberFd = memberIt->first;
							send(memberFd, response.c_str(), response.length(), 0);
						}
					}
					else if (parsed[2][1] == 'o') {
						int targetFd = _server->findClientByNickname(parsed[3]);
						if (targetFd == -1) {
							std::string response = ERR_NOSUCHNICK(userId, _server->getClient(fd).getNickname(), parsed[3]);
							send(fd, response.c_str(), response.length(), 0);
							return;
						}
						if (!chan->isMember(targetFd)) {
							std::string response = ERR_USERNOTINCHANNEL(userId, _server->getClient(fd).getNickname(), target, parsed[3]);
							send (fd, response.c_str(), response.length(), 0);
							return;
						}
						chan->removeOperator(targetFd);
						std::string response = userId + " MODE " + target + " -o " + parsed[3] + "\r\n";
						std::map<int, clients*> members = chan->getMembers();
						for (std::map<int, clients*>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
							int memberFd = memberIt->first;
							send(memberFd, response.c_str(), response.length(), 0);
						}
						// send (targetFd, response.c_str(), response.length(), 0);
					}
					else if (parsed[2][1] == 't') {
						channel* chan = _server->getChannel(target);
						chan->setReservedTopic(false);
						std::string response = userId + " MODE " + target + " -t\r\n";
						send (fd, response.c_str(), response.length(), 0);
						// chan->setTopic(chan->getTopic()); // No change, just a placeholder
					}
					else if (parsed[2][1] == 'k') {
						chan->setPasswordProtected(false);
						std::string response = userId + " MODE " + target + " -k\r\n";
						send (fd, response.c_str(), response.length(), 0);
						
					}
					else if (parsed[2][1] == 'l') {
						chan->setLimitEnabled(false);
						chan->setUserLimit(0);
						std::string response = userId + " MODE " + target + " -l\r\n";
						send (fd, response.c_str(), response.length(), 0);
					}
					else {
						std::string response = ERR_UNKNOWNMODE(userId, _server->getClient(fd).getNickname(), parsed[2]);
						send(fd, response.c_str(), response.length(), 0);
					}
				}
			}
		} else {
			std::string response = ERR_UMODEUNKNOWNFLAG(userId, _server->getClient(fd).getNickname());
			send(fd, response.c_str(), response.length(), 0);
			}
	}
}