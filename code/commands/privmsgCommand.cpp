#include "privmsgCommand.hpp"
#include "../server.hpp"

PrivmsgCommand::PrivmsgCommand(server* srv) : Command(srv) {
	
}

void PrivmsgCommand::execute(int fd, const std::vector<std::string>& parsed) {
    if (!_server->getClient(fd).isRegistered()) {
        std::cout << "You need to register before sending messages" << std::endl;
        return;
    }
    
    if (parsed.size() < 3) {
        std::cout << "Wrong number of arguments" << std::endl;
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
            std::cout << "No user called like that bro" << std::endl;
        }
    }
}
