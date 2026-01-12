
#include "irc_client.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <cstdlib>

#define BUFFER_SIZE 512

IRCClient::IRCClient(const std::string& ip, int port, const std::string& pass)
    : password(pass), sockfd(-1), serverIP(ip), serverPort(port), connected(false), botNickname("boop") {}

IRCClient::~IRCClient() {
    disconnect();
}

bool IRCClient::connectToServer() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return false;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);

    if (inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address" << std::endl;
        return false;
    }

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed" << std::endl;
        return false;
    }

    connected = true;
    std::cout << "Connected to " << serverIP << ":" << serverPort << std::endl;
    registerBot();

    return true;
}

void IRCClient::disconnect() {
    if (sockfd >= 0) {
        close(sockfd);
        sockfd = -1;
        connected = false;
        std::cout << "Disconnected from server" << std::endl;
    }
}

bool IRCClient::sendMessage(const std::string& message) {
    if (!connected) return false;

    std::string msg = message;
    if (msg.find("\r\n") == std::string::npos) {
        msg += "\r\n";
    }
    ssize_t sent = send(sockfd, msg.c_str(), msg.length(), 0);

    if (sent < 0) {
        std::cerr << "Send failed" << std::endl;
        return false;
    }
    return true;
}

void IRCClient::run() {
    if (!connected) return;

    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN;

    char buffer[BUFFER_SIZE];
    std::cout << "Client ready. Type your messages:\n";

    while (connected) {
        int ret = poll(fds, 2, -1);
        if (ret < 0) {
            std::cerr << "Poll error" << std::endl;
            break;
        }

        if (fds[0].revents & POLLIN) {
            std::string line;
            if (!std::getline(std::cin, line)) {
                break;
            }
            if (line == "/quit") {
                sendMessage("QUIT");
                break;
            }
            sendMessage(line);
        }

        if (fds[1].revents & POLLIN) {
            memset(buffer, 0, BUFFER_SIZE);
            ssize_t received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);

            if (received <= 0) {
                std::cout << "Server disconnected" << std::endl;
                break;
            }

            std::string serverMsg(buffer);
            std::cout << serverMsg;
            std::cout.flush();

            parseServerMessage(serverMsg);
        }
    }
}

void IRCClient::registerBot() {
    if (!password.empty()) {
        sendMessage("PASS " + password);
    }
    sendMessage("NICK " + botNickname);
    sendMessage("USER " + botNickname + " 0 * :Help Bot");
}

void IRCClient::parseServerMessage(const std::string& message) {
    size_t privmsgPos = message.find("PRIVMSG");
    if (privmsgPos == std::string::npos) return;

    std::istringstream iss(message);
    std::string token, sender, target, content;

    if (message[0] == ':') {
        iss >> token;
        size_t nickEnd = token.find('!');
        if (nickEnd != std::string::npos) {
            sender = token.substr(1, nickEnd - 1);
        }
    }

    iss >> token;
    iss >> target;

    if (iss.peek() == ' ') iss.ignore();
    if (iss.peek() == ':') {
        iss.ignore();
        std::getline(iss, content);
    } else {
        iss >> content;
    }

    std::string lowerContent = content;
    std::transform(lowerContent.begin(), lowerContent.end(),
                 lowerContent.begin(), ::tolower);

    if (lowerContent.find("help") != std::string::npos) {
        std::string replyTarget;
        if (!target.empty() && (target[0] == '#' || target[0] == '&')) {
            replyTarget = target;
        } else if (!sender.empty()) {
            replyTarget = sender; 
        } else {
            return;
        }

        sendHelpMessage(replyTarget);
    }
}

void IRCClient::sendHelpMessage(const std::string& target) {
    std::vector<std::string> commands;
    commands.push_back("Available IRC commands:");
    commands.push_back("NICK <nickname> - Change your nickname");
    commands.push_back("USER <username> <mode> <unused> <realname> - Set user information");
    commands.push_back("JOIN <channel> - Join a channel");
    commands.push_back("PART <channel> [message] - Leave a channel");
    commands.push_back("PRIVMSG <target> <message> - Send a message");
    commands.push_back("QUIT [message] - Disconnect from server");
    commands.push_back("TOPIC <channel> [topic] - View or set channel topic");
    commands.push_back("MODE <target> <modes> - Change channel/user modes");
    commands.push_back("KICK <channel> <user> [reason] - Kick a user");
    commands.push_back("INVITE <nickname> <channel> - Invite user to channel");
    commands.push_back("WHO <mask> - Get information about users");
    commands.push_back("WHOIS <nickname> - Get detailed user information");

    for (size_t i = 0; i < commands.size(); ++i) {
        sendMessage("PRIVMSG " + target + " :" + commands[i]);
    }
}