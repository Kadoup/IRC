#include "topicCommand.hpp"
#include "../server.hpp"

TopicCommand::TopicCommand(server* srv) : Command(srv) {
	
}

void TopicCommand::execute(int fd, const std::vector<std::string>& parsed) {
    std::string channelName = parsed[1];
    std::map<std::string, channel>& channels = _server->getChannels();
    std::map<std::string, channel>::iterator it = channels.find(channelName);
    std::string userId = USER_IDENTIFIER(_server->getClient(fd).getNickname(), _server->getClient(fd).getUsername());

    if (it == channels.end()) {
        std::string response = ERR_NOSUCHCHANNEL(userId, _server->getClient(fd).getNickname(), channelName);
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    if (parsed.size() < 2) {
        std::string response = ERR_NEEDMOREPARAMS(userId, _server->getClient(fd).getNickname(), "TOPIC");
        send(fd, response.c_str(), response.length(), 0);
        return;
    }
    if (parsed.size() == 2) {
        std::string topic = it->second.getTopic();
        std::string response = RPL_TOPIC(userId, _server->getClient(fd).getNickname(), channelName, topic);
        send(fd, response.c_str(), response.length(), 0);
    }
    else if (parsed.size() == 3) {
        if (!it->second.isOperator(fd)) {
            std::string response = ERR_CHANOPRIVSNEEDED(userId, _server->getClient(fd).getNickname(), channelName);
            send(fd, response.c_str(), response.length(), 0);
            return;
        }
        it->second.setTopic(parsed[2]);
        std::string response = userId + " TOPIC " + channelName + " :" + parsed[2] + "\r\n";
        std::map<int, clients*> members = it->second.getMembers();
        for (std::map<int, clients*>::iterator memberIt = members.begin(); 
             memberIt != members.end(); ++memberIt) {
            int memberFd = memberIt->first;
            send(memberFd, response.c_str(), response.length(), 0);
        }
    }
    else {
        std::string response = ERR_NEEDMOREPARAMS(userId, _server->getClient(fd).getNickname(), "TOPIC");
        send(fd, response.c_str(), response.length(), 0);
    }
}
