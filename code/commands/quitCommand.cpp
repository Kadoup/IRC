#include "quitCommand.hpp"
#include "../server.hpp"

QuitCommand::QuitCommand(server* srv) : Command(srv) {
	
}

void QuitCommand::execute(int fd, const std::vector<std::string>& parsed) {
    if (parsed.size() > 2) {
        std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
        std::string response = ERR_NEEDMOREPARAMS(userId, _server->getClient(fd).getNickname(), "QUIT");
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    std::string quitMessage = parsed.size() == 2 ? parsed[1] : "Client Quit";
    std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
    std::string response = userId + " QUIT :" + quitMessage + "\r\n";
    close(fd);
    _server->removeClient(fd);
}
