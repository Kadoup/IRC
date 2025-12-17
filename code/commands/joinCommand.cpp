#include "joinCommand.hpp"
#include "../server.hpp"

JoinCommand::JoinCommand(server* srv) : Command(srv) {
	
}

bool JoinCommand::isInvited(int fd, channel& chan) {
    std::map<int, clients*> invited = chan.getInvited();
    if (invited.find(fd) == invited.end()) {
        std::string response = ":localhost 473 " + _server->getClient(fd).getNickname() +
                            " " + chan.getName() + " :Cannot join channel (+i)\r\n";
        send(fd, response.c_str(), response.length(), 0);
        return false;
    }
    return true;
}


//impl password later
void JoinCommand::execute(int fd, const std::vector<std::string>& parsed) {
    
    std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
    if (parsed.size() != 2 && parsed.size() != 3) {
        std::string response = ERR_NEEDMOREPARAMS(userId, _server->getClient(fd).getNickname(), "JOIN");
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    if (!_server->getClient(fd).isRegistered()) {
        std::string response = ERR_NOTREGISTERED(userId, _server->getClient(fd).getNickname());
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    char prefix = parsed[1][0];
    if (prefix != '#' && prefix != '&' && prefix != '!' && prefix != '+') {
        std::string response = ERR_BADCHANMASK(userId, _server->getClient(fd).getNickname(), parsed[1]);
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    if (parsed[1].length() > 50) {
        std::string response = ERR_BADCHANMASK(userId, _server->getClient(fd).getNickname(), parsed[1]);
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    
    for (size_t i = 1; i < parsed[1].length(); ++i) {
        char c = parsed[1][i];
        if (!std::isalnum(c) && c != '-' &&
            c != '[' && c != ']' && c != '\\' && 
            c != '`' && c != '_' && c != '^' && 
            c != '{' && c != '|' && c != '}') {
            return;
        }
    }
    if (_server->getChannels().find(parsed[1]) != _server->getChannels().end()) {
        channel& chan = _server->getChannels()[parsed[1]];
        if (chan.isMember(fd)) {
            std::string response = ERR_USERONCHANNEL(userId, _server->getClient(fd).getNickname(), parsed[1], _server->getClient(fd).getNickname());
            send(fd, response.c_str(), response.length(), 0);
            return;
        }
        if (chan.getInviteOnly() && !isInvited(fd, chan)) {
            std::string response = ERR_INVITEONLYCHAN(userId, _server->getClient(fd).getNickname(), parsed[1]);
            send(fd, response.c_str(), response.length(), 0);
            return;
        }
        if (chan.limitReached())
        {
            std::string response = ":localhost 471 " + _server->getClient(fd).getNickname() +
                                " " + chan.getName() + " :Cannot join channel (+l)\r\n";
            send(fd, response.c_str(), response.length(), 0);
            return;
        }
        if (chan.getPasswordProtected()) {
            if (parsed.size() < 3 || parsed[2] != chan.getPassword()) {
                std::string response = ":localhost 475 " + _server->getClient(fd).getNickname() +
                                    " " + chan.getName() + " :Cannot join channel (+k)\r\n";
                send(fd, response.c_str(), response.length(), 0);
                return;
            }
        }
    }
    std::map<std::string, channel>& channels = _server->getChannels();
    if (channels.find(parsed[1]) != channels.end()) {
        channels[parsed[1]].addMember(fd, &_server->getClient(fd));
    } else {
        std::cout << "Creating channel: " << parsed[1] << std::endl;
        std::cout << "Client nickname: " << _server->getClient(fd).getNickname() << std::endl;
        std::cout << "Client username: " << _server->getClient(fd).getUsername() << std::endl;
        channel newChannel(parsed[1], &_server->getClient(fd));
        channels[parsed[1]] = newChannel;
    }
    std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
    std::string response = userId + " JOIN " + parsed[1] + "\r\n";
    send(fd, response.c_str(), response.length(), 0);
    
    std::string topic = channels[parsed[1]].getTopic();
    std::cout << "Topic for channel " << parsed[1] << ": " << topic << std::endl;
    response = RPL_TOPIC(userId, _server->getClient(fd).getNickname(), parsed[1], topic);
    send(fd, response.c_str(), response.length(), 0);
    
    std::string namesList;
    std::map<int, clients*> members = channels[parsed[1]].getMembers();
    std::map<int, clients*>::iterator memberIt;
    for (memberIt = members.begin(); memberIt != members.end(); ++memberIt) {
        if (channels[parsed[1]].isOperator(memberIt->first))
            namesList += "@";
        namesList += memberIt->second->getNickname();
        namesList += " ";
    }
    // std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());
    // response = userId + " JOIN " + parsed[1] + "\r\n";
    send(fd, response.c_str(), response.length(), 0);
    response = RPL_NAMREPLY(userId, _server->getClient(fd).getNickname(), parsed[1], namesList);
    send(fd, response.c_str(), response.length(), 0);
    response = RPL_ENDOFNAMES(userId, _server->getClient(fd).getNickname(), parsed[1]);
    send(fd, response.c_str(), response.length(), 0);
}
