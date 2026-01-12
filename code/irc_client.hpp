
#pragma once

#include <string>
#include <vector>

class IRCClient {
private:
    std::string password;
    int sockfd;
    std::string serverIP;
    int serverPort;
    bool connected;
    std::string botNickname;

    void registerBot();
    void parseServerMessage(const std::string& message);
    void sendHelpMessage(const std::string& target);

public:
    IRCClient(const std::string& ip, int port, const std::string& pass);
    ~IRCClient();

    bool connectToServer();
    void disconnect();
    bool sendMessage(const std::string& message);
    void run();
};