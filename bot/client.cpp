#include "irc_client.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <port> <password>" << std::endl;
        return 1;
    }

    std::string serverIP = argv[1];
    int port = std::atoi(argv[2]);
    std::string password = argv[3];

    IRCClient client(serverIP, port, password);

    if (!client.connectToServer()) {
        return 1;
    }

    client.run();

    return 0;
}