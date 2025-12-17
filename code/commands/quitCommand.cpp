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
    std::cout << "Client " << fd << " is quitting: " << quitMessage << std::endl;

    std::string errorMsg = "ERROR :Closing connection\r\n";
    send(fd, errorMsg.c_str(), errorMsg.length(), 0);
    
    clients& client = _server->getClient(fd);
    std::string nickname = client.getNickname();
    
    if (!nickname.empty()) {
        std::string quitMsg = ":" + nickname + " QUIT :" + quitMessage + "\r\n";
        std::map<std::string, channel>& channels = _server->getChannels();
        
        for (std::map<std::string, channel>::iterator it = channels.begin(); 
             it != channels.end(); ++it) {
            if (it->second.isMember(fd)) {
                it->second.broadcastMessage(fd, quitMsg);
                it->second.removeMember(fd);
            }
        }
    }
    std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
    std::string response = userId + " QUIT :" + quitMessage + "\r\n";
    _server->removeClient(fd);
    close(fd);
}
