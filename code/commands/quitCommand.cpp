#include "quitCommand.hpp"
#include "../server.hpp"

QuitCommand::QuitCommand(server* srv) : Command(srv) {
	
}

void QuitCommand::execute(int fd, const std::vector<std::string>& parsed) {
    std::string quitMessage;
    
    if (parsed.size() > 1) {
        quitMessage = parsed[1];
    } else {
        quitMessage = "Client quit";
    }
    
    std::cout << "Client " << fd << " is quitting: " << quitMessage << std::endl;

    std::string errorMsg = "ERROR :Closing connection\r\n";
    send(fd, errorMsg.c_str(), errorMsg.length(), 0);
    
    // Notify channels the user was in
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
    _server->removeClient(fd);
    close(fd);
}
