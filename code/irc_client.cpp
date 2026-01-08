
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
    : sockfd(-1), serverIP(ip), serverPort(port), connected(false), password(pass), botNickname("boop") {}

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

        // Check for user input
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

        // Check for server messages
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

            // Parse for help commands
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

    // Parse prefix if exists
    if (message[0] == ':') {
        iss >> token; // :nick!user@host
        size_t nickEnd = token.find('!');
        if (nickEnd != std::string::npos) {
            sender = token.substr(1, nickEnd - 1);
        }
    }

    iss >> token; // PRIVMSG
    iss >> target; // target (channel or nickname)

    // Get the rest as message content
    if (iss.peek() == ' ') iss.ignore();

    // Check if next part starts with :
    if (iss.peek() == ':') {
        iss.ignore(); // skip the :
        std::getline(iss, content); // rest of line
    } else {
        iss >> content; // single word without :
    }

    // Convert to lowercase for case-insensitive comparison
    std::string lowerContent = content;
    std::transform(lowerContent.begin(), lowerContent.end(),
                 lowerContent.begin(), ::tolower);

    if (lowerContent.find("help") != std::string::npos) {
        // Determine where to reply
        std::string replyTarget;
        if (!target.empty() && (target[0] == '#' || target[0] == '&')) {
            replyTarget = target; // reply to channel
        } else if (!sender.empty()) {
            replyTarget = sender; // reply to sender
        } else {
            return; // no valid reply target
        }

        sendHelpMessage(replyTarget);
    }
}

void IRCClient::sendHelpMessage(const std::string& target) {
    std::vector<std::string> commands = {
        "Available IRC commands:",
        "NICK <nickname> - Change your nickname",
        "USER <username> <mode> <unused> <realname> - Set user information",
        "JOIN <channel> - Join a channel",
        "PART <channel> [message] - Leave a channel",
        "PRIVMSG <target> <message> - Send a message",
        "QUIT [message] - Disconnect from server",
        "TOPIC <channel> [topic] - View or set channel topic",
        "MODE <target> <modes> - Change channel/user modes",
        "KICK <channel> <user> [reason] - Kick a user",
        "INVITE <nickname> <channel> - Invite user to channel",
        "WHO <mask> - Get information about users",
        "WHOIS <nickname> - Get detailed user information"
    };

    for (const auto& cmd : commands) {
        sendMessage("PRIVMSG " + target + " :" + cmd);
    }
}