#include "nickCommand.hpp"
#include "../server.hpp"

NickCommand::NickCommand(server* srv) : Command(srv) {
	
}

void NickCommand::execute(int fd, const std::vector<std::string>& parsed) {
    std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
    if (parsed.size() != 2) {
        std::string response = ERR_NEEDMOREPARAMS(userId, _server->getClient(fd).getNickname(), "NICK");
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    const std::string& param = parsed[1];
    if (param.empty() || param.length() > 9) {
        std::string response = ERR_ERRONEUSNICKNAME(userId, _server->getClient(fd).getNickname(), parsed[1]);
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    char first = param[0];
    if (!std::isalnum(first) &&
        first != '[' && first != ']' && first != '\\' && 
        first != '`' && first != '_' && first != '^' && 
        first != '{' && first != '|' && first != '}') {
        std::string response = ERR_ERRONEUSNICKNAME(userId, _server->getClient(fd).getNickname(), parsed[1]);
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    for(size_t i = 1; i < param.length(); ++i) {
        char c = param[i];
        if (!std::isalnum(c) && c != '-' &&
            c != '[' && c != ']' && c != '\\' && 
            c != '`' && c != '_' && c != '^' && 
            c != '{' && c != '|' && c != '}') {
            std::string response = ERR_ERRONEUSNICKNAME(userId, _server->getClient(fd).getNickname(), parsed[1]);
            send(fd, response.c_str(), response.length(), 0);
            return;
        }
    }
    if (_server->getClient(fd).isRegistered()) {
        std::string response = ERR_NICKNAMEINUSE(userId, _server->getClient(fd).getNickname(), parsed[1]);
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    if (!_server->isUniqueNickname(parsed[1])) {
        std::string response = ERR_NICKNAMEINUSE(userId, _server->getClient(fd).getNickname(), parsed[1]);
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
    if (!_server->getClient(fd).getNickname().empty()) {
        std::string response = userId + " NICK " + parsed[1] + "\r\n";
        send(fd, response.c_str(), response.length(), 0);
    }
    _server->getClient(fd).setNickname(parsed[1]);
}
