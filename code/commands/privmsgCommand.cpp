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
    
    // std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
    // std::string prefix;
    // prefix = ":" + _server->getClient(fd).getNickname() + "!" + 
    //          _server->getClient(fd).getUsername() + "@" + 
    //          _server->getClient(fd).getHostname() + " ";
    std::string target = parsed[1];
    std::vector<int> targetFds = _server->findTarget(target, fd);
    
    for (size_t i = 0; i < targetFds.size(); ++i) {
        std::string singleTarget = target.substr(0, target.find(','));
        while (targetFds[i] != -2) {
            if (!target.empty() && (target[0] == '#' || target[0] == '&' || 
                            target[0] == '!' || target[0] == '+')) {
                channel* chan = _server->getChannel(target);
                if (chan && !chan->isMember(fd)) {
                    std::string response = ERR_CANNOTSENDTOCHAN(userId, _server->getClient(fd).getNickname(), target);
                    send(fd, response.c_str(), response.length(), 0);
                    return;
                }
            }
            if (targetFds[i] != -1) {
                if (target.find(',') != std::string::npos)
                    target = target.substr(target.find(',') + 1);
                std::string message = userId + " " + parsed[0] + " " + singleTarget + " :" + parsed[2] + "\r\n";
                int sent = send(targetFds[i], message.c_str(), message.length(), 0);
                if (sent == -1)
                    std::cout << "Send failed" << std::endl;
            } else {
                std::string response = ERR_NOSUCHNICK(userId, _server->getClient(fd).getNickname(), target);
                send(fd, response.c_str(), response.length(), 0);
            }
            i++;
        }
    }
}
