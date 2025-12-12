#include "privmsgCommand.hpp"
#include "../server.hpp"

PrivmsgCommand::PrivmsgCommand(server* srv) : Command(srv) {
	
}

void PrivmsgCommand::execute(int fd, const std::vector<std::string>& parsed) {
    std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
    if (!_server->getClient(fd).isRegistered()) {
        std::string response = ERR_NOTREGISTERED(userId, _server->getClient(fd).getNickname());
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    if (parsed.size() < 2) {
        std::string response = ERR_NORECIPIENT(userId, _server->getClient(fd).getNickname(), "PRIVMSG");
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    if (parsed.size() < 3) {
        std::string response = ERR_NOTEXTTOSEND(userId, _server->getClient(fd).getNickname());
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    std::string prefix;
    prefix = ":" + _server->getClient(fd).getNickname() + "!" + 
             _server->getClient(fd).getUsername() + "@" + 
             _server->getClient(fd).getHostname() + " ";
    std::string target = parsed[1];
    std::string message = prefix + parsed[0] + " " + target + " :" + parsed[2] + "\r\n";
    std::vector<int> targetFds = _server->findTarget(target, fd);
    
    for (size_t i = 0; i < targetFds.size(); ++i) {
        if (targetFds[i] != -1) {
            int sent = send(targetFds[i], message.c_str(), message.length(), 0);
            if (sent == -1)
                std::cout << "Send failed" << std::endl;
        } else {
            std::string response = ERR_NOSUCHNICK(userId, _server->getClient(fd).getNickname(), target);
            send(fd, response.c_str(), response.length(), 0);
        }
    }
}
