#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <poll.h>
#include <cstdlib>
#include <sstream>
#include "clients.hpp"

class server
{
private:
    std::vector<pollfd> _fds;
    int _serverfd;
    int _port;
    std::string _password;
public:
    void initServer();
    void runServer();
    void broadcastMessage(int senderfd, const std::string& message);
    void connectClient();
    void handleClientMessage(size_t& i);
    server(int port, std::string password);
    ~server();
};

