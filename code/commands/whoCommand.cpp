#include "whoCommand.hpp"
#include "../server.hpp"
#include "../channel.hpp"

WhoCommand::WhoCommand(server* srv) : Command(srv) {

}

void WhoCommand::execute(int fd, const std::vector<std::string>& parsed) {
    std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
    if (parsed.size() < 2) {
        std::string response = ERR_NEEDMOREPARAMS(userId, _server->getClient(fd).getNickname(), "WHO");
        send(fd, response.c_str(), response.length(), 0);
        return;
    }

    std::string target = parsed[1];
    std::vector<int> targetFds = _server->findTarget(target, fd);
    for (size_t i = 0; i < targetFds.size(); ++i) {
        clients& targetClient = _server->getClient(targetFds[i]);
        std::string response = RPL_WHOREPLY(userId, target, targetClient.getUsername(), targetClient.getHostname(), _server->_getServerName(), targetClient.getNickname(), "H", "0");
        send(fd, response.c_str(), response.length(), 0);
    }
    std::string endOfNamesResponse = RPL_ENDOFNAMES(userId, _server->getClient(fd).getNickname(), target);
    send(fd, endOfNamesResponse.c_str(), endOfNamesResponse.length(), 0);
}