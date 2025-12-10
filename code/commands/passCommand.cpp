#include "passCommand.hpp"
#include "../server.hpp"

PassCommand::PassCommand(server* srv) : Command(srv) {}

void PassCommand::execute(int fd, const std::vector<std::string>& parsed) {
    if (parsed.size() != 2) {
        std::cout << "Wrong number of arguments" << std::endl;
        return;
    }
    
    if (_server->getClient(fd).isAuthentificated()) {
        std::cout << "You are already authenticated" << std::endl;
        return;
    }
    
    if (parsed[1] == _server->getPassword()) {
        std::cout << "Welcome " << _server->getClient(fd).getNickname() << std::endl;
        _server->getClient(fd).setAuthentificated(true);
		_server->getClient(fd).checkRegistration();
    } else {
        std::cout << "Wrong Password" << std::endl;
    }
}
