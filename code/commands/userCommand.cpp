#include "userCommand.hpp"
#include "../server.hpp"

UserCommand::UserCommand(server* srv) : Command(srv) {
	
}

void UserCommand::execute(int fd, const std::vector<std::string>& parsed) {
    if (parsed.size() != 5) {
        std::cout << "Wrong number of arguments" << std::endl;
        return;
    }
    
    const std::string& param = parsed[1];
    if (param.empty() || param.length() > 9) {
        std::cout << "Bad Username" << std::endl;
        return;
    }
    
    for(size_t i = 0; i < param.length(); ++i) {
        char c = param[i];
        if (!std::isalnum(c) && c != '-' &&
            c != '[' && c != ']' && c != '\\' && 
            c != '`' && c != '_' && c != '^' && 
            c != '{' && c != '|' && c != '}') {
            std::cout << "Bad Username" << std::endl;
            return;
        }
    }
    
    _server->getClient(fd).setUsername(parsed[1]);
    _server->getClient(fd).setRealName(parsed[4]);
}
