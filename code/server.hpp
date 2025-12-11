#pragma once

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <cstdlib>
#include <map>
#include <algorithm>
#include <sstream>
#include "clients.hpp"
#include "channel.hpp"
#include "numReplies.hpp"

class Command;

class server
{
private:
    std::vector<pollfd> _fds;
    std::map<int, clients> _clients;
    std::map<std::string, channel> _channels;
    std::map<std::string, Command*> _commands;
    int _serverfd;
    int _port;
    std::string _password;
    std::string _serverName;
    
    void registerCommands();
    
public:
    void initServer();
    void runServer();
    void broadcastMessage(int senderfd, const std::string& message);
    void connectClient();
    void handleClientMessage(size_t& i);
	channel* getChannel(const std::string& name);
    void handleCommands(int fd, const std::vector<std::string>& parsed);
    bool isUniqueNickname(std::string nick);
    std::string _getServerName() const;
    std::vector<int> findTarget(std::string target, int senderFd);
	int findClientByNickname(const std::string& nickname);
    std::string getPassword() const;
    clients& getClient(int fd);
    std::map<int, clients>& getClients();
    std::map<std::string, channel>& getChannels();
    void removeClient(int fd);
    
    server(int port, std::string password);
    ~server();
};

