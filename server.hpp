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

class server
{
private:
    std::vector<pollfd> _fds;
    std::map<int, clients> _clients;
    std::map<std::string, channel> _channels;
    int _serverfd;
    int _port;
    std::string _password;
public:
    void initServer();
    void runServer();
    void broadcastMessage(int senderfd, const std::string& message);
    void connectClient();
    void handleClientMessage(size_t& i);
    void handleNick(int fd, std::vector<std::string> parsed);
    void handleCommands(int fd, std::vector<std::string> parsed);
    bool isUniqueNickname(std::string nick);
    void handlePrvMsg(int fd, std::vector<std::string> parsed);
    void handleUser(int fd, std::vector<std::string> parsed);
    void handlePass(int fd, std::vector<std::string> parsed);
    void handleQuit(int fd, std::vector<std::string> parsed);
    void handleJoin(int fd, std::vector<std::string>& parsed);
    std::vector<int> findTarget(std::string target);
    server(int port, std::string password);
    ~server();
};

