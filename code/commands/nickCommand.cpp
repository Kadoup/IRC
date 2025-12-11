#include "nickCommand.hpp"
#include "../server.hpp"

NickCommand::NickCommand(server* srv) : Command(srv) {
	
}

void NickCommand::execute(int fd, const std::vector<std::string>& parsed) {
    if (parsed.size() != 2) {
        std::cout << "Wrong number of arguments" << std::endl;
        return;
    }
    
    const std::string& param = parsed[1];
    if (param.empty() || param.length() > 9) {
        std::cout << "Bad nickname" << std::endl;
        return;
    }
    
    char first = param[0];
    if (!std::isalnum(first) &&
        first != '[' && first != ']' && first != '\\' && 
        first != '`' && first != '_' && first != '^' && 
        first != '{' && first != '|' && first != '}') {
        std::cout << "Bad nickname" << std::endl;
        return;
    }
    
    for(size_t i = 1; i < param.length(); ++i) {
        char c = param[i];
        if (!std::isalnum(c) && c != '-' &&
            c != '[' && c != ']' && c != '\\' && 
            c != '`' && c != '_' && c != '^' && 
            c != '{' && c != '|' && c != '}') {
            std::cout << "Bad nickname" << std::endl;
            return;
        }
    }
    
    if (!_server->isUniqueNickname(parsed[1])) {
        std::cout << "Nickname is not unique" << std::endl;
        return;
    }
    
    _server->getClient(fd).setNickname(parsed[1]);
}
