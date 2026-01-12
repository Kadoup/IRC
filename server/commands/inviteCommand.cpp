#include "inviteCommand.hpp"
#include "../server.hpp"

InviteCommand::InviteCommand(server* srv) : Command(srv) {

}

void InviteCommand::execute(int fd, const std::vector<std::string>& parsed) {
	std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
	if (!_server->getClient(fd).isRegistered()) {
		std::string response = ERR_NOTREGISTERED(userId, _server->getClient(fd).getNickname());
		send(fd, response.c_str(), response.length(), 0);
		return;
	}
	if (parsed.size() != 3) {
		std::string response = ERR_NEEDMOREPARAMS(userId, _server->getClient(fd).getNickname(), "INVITE");
		send(fd, response.c_str(), response.length(), 0);
		return;
	}
	std::string channelName = parsed[2];
	std::string inviteeNick = parsed[1];
	std::map<std::string, channel>& channels = _server->getChannels();
	std::map<std::string, channel>::iterator chanIt = channels.find(channelName);

	if (chanIt == channels.end()) {
		std::string response = ERR_NOSUCHCHANNEL(userId, _server->getClient(fd).getNickname(), channelName);
		send(fd, response.c_str(), response.length(), 0);
		return;
	}
	if (!chanIt->second.isOperator(fd)) {
		std::string response = ERR_CHANOPRIVSNEEDED(userId, _server->getClient(fd).getNickname(), channelName);
		send(fd, response.c_str(), response.length(), 0);
		return;
	}
	int inviteeFd = _server->findClientByNickname(inviteeNick);
	if (inviteeFd == -1) {
		std::string response = ERR_NOSUCHNICK(userId, _server->getClient(fd).getNickname(), inviteeNick);
		send(fd, response.c_str(), response.length(), 0);
		return;
	}
	chanIt->second.addInvite(inviteeFd, &_server->getClient(inviteeFd));
	std::string response = userId + " INVITE " + inviteeNick + " " + channelName + "\r\n";
	send(inviteeFd, response.c_str(), response.length(), 0);
	std::string reply = RPL_INVITING(userId, _server->getClient(fd).getNickname(), channelName, inviteeNick);
	send(fd, reply.c_str(), reply.length(), 0);
}