#include "capCommand.hpp"
#include "../server.hpp"
#include "../clients.hpp"
#include <sys/socket.h>

CapCommand::CapCommand(server* srv) : command(srv) {
    
}

void CapCommand::execute(int fd, const std::vector<std::string>& parsed) {
    clients& client = _server->getClient(fd);
    
    if (parsed.size() >= 2 && parsed[1] == "LS") {
        std::string response = ":" + _server->_getServerName() + " CAP * LS :\r\n";
        send(fd, response.c_str(), response.length(), 0);
    }
    else if (parsed.size() >= 3 && parsed[1] == "REQ") {
        std::string requestedFeatures = parsed[2];
        if (requestedFeatures[0] == ':')
            requestedFeatures = requestedFeatures.substr(1);
        
        for (size_t i = 3; i < parsed.size(); i++) {
            requestedFeatures += " " + parsed[i];
        }
        
        std::string response = ":" + _server->_getServerName() + " CAP * NAK :" + requestedFeatures + "\r\n";
        send(fd, response.c_str(), response.length(), 0);
    }
    else if (parsed.size() >= 2 && parsed[1] == "END") {
        // Just acknowledge, do nothing special
        return;
    }
}