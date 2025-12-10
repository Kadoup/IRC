#include "kickCommand.hpp"
#include "../server.hpp"

KickCommand::KickCommand(server* srv) : Command(srv) {}

void KickCommand::execute(int fd, const std::vector<std::string>& parsed) {
    if (parsed.size() < 3) {
        std::cout << "Wrong number of arguments" << std::endl;
        return;
    }
    
    std::string channelName = parsed[1];
    std::string targetNick = parsed[2];
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
    
    int targetFd = -1;
    std::map<int, clients>::iterator clientIt;
    for (clientIt = _server->getClients().begin(); 
         clientIt != _server->getClients().end(); ++clientIt) {
        if (clientIt->second.getNickname() == targetNick) {
            targetFd = clientIt->first;
            break;
        }
    }

    if (targetFd == -1) {
        std::cout << "User not found" << std::endl;
        return;
    }

    if (!chanIt->second.isMember(targetFd)) {
        std::cout << "User is not in the channel" << std::endl;
        return;
    }

    chanIt->second.removeMember(targetFd);

    std::string reason = parsed.size() > 3 ? parsed[3] : targetNick;
    std::string response = ":" + _server->getClient(fd).getNickname() + "!" + 
                          _server->getClient(fd).getUsername() + "@localhost KICK " + 
                          channelName + " " + targetNick + " :" + reason + "\r\n";

    std::map<int, clients*> members = chanIt->second.getMembers();
    for (std::map<int, clients*>::iterator it = members.begin(); it != members.end(); ++it) {
        send(it->first, response.c_str(), response.length(), 0);
    }
    send(targetFd, response.c_str(), response.length(), 0);
}
