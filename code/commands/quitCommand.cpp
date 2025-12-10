#include "quitCommand.hpp"
#include "../server.hpp"

QuitCommand::QuitCommand(server* srv) : Command(srv) {}

void QuitCommand::execute(int fd, const std::vector<std::string>& parsed) {
    if (parsed.size() > 2) {
        std::cout << "Wrong number of arguments" << std::endl;
        return;
    }
    
    close(fd);
    _server->removeClient(fd);
}
