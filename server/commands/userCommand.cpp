#include "userCommand.hpp"
#include "../server.hpp"

UserCommand::UserCommand(server* srv) : Command(srv) {
	
}

void UserCommand::execute(int fd, const std::vector<std::string>& parsed) {
    if (parsed.size() != 5) {
        std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
        std::string response = ERR_NEEDMOREPARAMS(userId, _server->getClient(fd).getNickname(), "USER");
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    const std::string& param = parsed[1];
    if (param.empty() || param.length() > 9) {
        std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
        std::string response = ERR_ERRONEUSNICKNAME(userId, _server->getClient(fd).getNickname(), param);
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    for(size_t i = 0; i < param.length(); ++i) {
        char c = param[i];
        if (!std::isalnum(c) && c != '-' &&
            c != '[' && c != ']' && c != '\\' && 
            c != '`' && c != '_' && c != '^' && 
            c != '{' && c != '|' && c != '}') {
            std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
            std::string response = ERR_ERRONEUSNICKNAME(userId, _server->getClient(fd).getNickname(), param);
            send(fd, response.c_str(), response.length(), 0);
            return;
        }
    }
    
    _server->getClient(fd).setUsername(parsed[1]);
    _server->getClient(fd).setRealName(parsed[4]);
}
