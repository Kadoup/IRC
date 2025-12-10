#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <cstdlib>

#define BUFFER_SIZE 512

class IRCClient {
private:
    int sockfd;
    std::string serverIP;
    int serverPort;
    bool connected;

public:
    IRCClient(const std::string& ip, int port) 
        : sockfd(-1), serverIP(ip), serverPort(port), connected(false) {}

    ~IRCClient() {
        disconnect();
    }

    bool connectToServer() {
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
        return true;
    }

    void disconnect() {
        if (sockfd >= 0) {
            close(sockfd);
            sockfd = -1;
            connected = false;
            std::cout << "Disconnected from server" << std::endl;
        }
    }

    bool sendMessage(const std::string& message) {
        if (!connected) return false;
        
        std::string msg = message + "\r\n";
        ssize_t sent = send(sockfd, msg.c_str(), msg.length(), 0);
        
        if (sent < 0) {
            std::cerr << "Send failed" << std::endl;
            return false;
        }
        return true;
    }

    void run() {
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
                
                std::cout << buffer;
                std::cout.flush();
            }
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <port>" << std::endl;
        return 1;
    }

    std::string serverIP = argv[1];
    int port = std::atoi(argv[2]);

    IRCClient client(serverIP, port);
    
    if (!client.connectToServer()) {
        return 1;
    }

    client.run();
    
    return 0;
}