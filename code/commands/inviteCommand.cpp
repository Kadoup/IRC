#include "inviteCommand.hpp"
#include "../server.hpp"

InviteCommand::InviteCommand(server* srv) : Command(srv) {

}

void InviteCommand::execute(int fd, const std::vector<std::string>& parsed) {
	if (!_server->getClient(fd).isRegistered()) {
		std::cout << "You are not registered" << std::endl;
		return;
	}
	if (parsed.size() != 3) {
		std::cout << "Wrong number of arguments" << std::endl;
		return;
	}
	std::string channelName = parsed[1];
	std::string inviteeNick = parsed[2];
	std::map<std::string, channel>& channels = _server->getChannels();
	std::map<std::string, channel>::iterator chanIt = channels.find(channelName);

	if (chanIt == channels.end()) {
		std::cout << "Channel does not exist" << std::endl;
		return;
	}
	if (!chanIt->second.isOperator(fd)) {
		std::cout << "You are not channel operator" << std::endl;
		return;
	}
	int inviteeFd = _server->findClientByNickname(inviteeNick);
	if (inviteeFd == -1) {
		std::cout << "User not found" << std::endl;
		return;
	}
	chanIt->second.addInvite(inviteeFd, &_server->getClient(inviteeFd));
	std::string response = ":" + _server->getClient(fd).getNickname() + "!" +
						   _server->getClient(fd).getUsername() + "@localhost INVITE " +
						   inviteeNick + " :" + channelName + "\r\n";
	send(inviteeFd, response.c_str(), response.length(), 0);
	std::string reply = ":" + _server->_getServerName() + " 341 " + 
						_server->getClient(fd).getNickname() + " " +
						inviteeNick + " " + channelName + "\r\n";
	send(fd, reply.c_str(), reply.length(), 0);
}