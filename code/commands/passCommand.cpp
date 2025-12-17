#include "passCommand.hpp"
#include "../server.hpp"

PassCommand::PassCommand(server* srv) : Command(srv) {
	
}

void PassCommand::execute(int fd, const std::vector<std::string>& parsed) {
    std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
    if (parsed.size() != 2) {
        std::string response = ERR_NEEDMOREPARAMS(userId, _server->getClient(fd).getNickname(), "PASS");
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    if (_server->getClient(fd).isAuthentificated()) {
        std::string response = ERR_ALREADYREGISTRED(userId, _server->getClient(fd).getNickname());
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    if (parsed[1] == _server->getPassword()) {
        _server->getClient(fd).setAuthentificated(true);
		_server->getClient(fd).checkRegistration();
    } else {
        std::string response = ERR_PASSWDMISMATCH(userId, _server->getClient(fd).getNickname());
        send(fd, response.c_str(), response.length(), 0);
    }
}
