#include "main.hpp"

pollfd    createPoll(int fd)
{
    pollfd newPollFd;
    newPollFd.fd = serverfd;
    serverPollFd.events = POLLIN;
}

void    connectClient(int serverfd, pollfd fds)
{
    int clientSocket = accept(serverfd, NULL, NULL);
    if (clientSocket != -1)
    {
        pollfd clientPollFd = createPoll(clientSocket);
        fds.push_back(clientPollFd);
        std::cout << "New client connected: " << clientSocket << std::endl;
    }
}

int main(int argc, char **argv)
{
    (void)argc;
    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverfd == -1)
    {
        std::cerr << "Socket creation failed" << std::endl;
        return 1;
    }
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(std::atoi(argv[1]));
    if (bind(serverfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Bind failed" << std::endl;
        close(serverfd);
        return 1;
    }
    if (listen(serverfd, SOMAXCONN) == -1)
    {
        std::cerr << "Listen failed" << std::endl;
        close(serverfd);
        return 1;
    }
    std::vector<pollfd> fds;
    pollfd serverPollFd = createPoll(serverfd);
    fds.push_back(serverPollFd);
    
    while (1)
    {
        if (poll(fds.data(), fds.size(), -1) < 0)
        {
            std::cerr << "Poll error" << std::endl;
            break;
        }
        for (size_t i = 0; i < fds.size(); ++i)
        {
            if (fds[i].revents & POLLIN)
            {
                if (fds[i].fd == serverfd)
                {
                    int clientSocket = accept(serverfd, NULL, NULL);
                    if (clientSocket != -1)
                    {
                        pollfd clientPollFd = createPoll(clientSocket);
                        fds.push_back(clientPollFd);
                        std::cout << "New client connected: " << clientSocket << std::endl;
                    }
                }
                else {
                    char buffer[1024];
                    int n = recv(fds[i].fd, buffer, sizeof(buffer), 0);
                    if (n <= 0)
                    {
                        std::cout << "Client disconnected: " << fds[i].fd << std::endl;
                        close(fds[i].fd);
                        fds.erase(fds.begin() + i);
                        i--;
                    }
                    else if (n > 0)
                    {
                        buffer[n] = '\0';
                        std::stringstream ss;
                        ss << fds[i].fd;
                        std::string clientName = "Client " + ss.str() + ": ";
                        std::string newBuffer = clientName + std::string(buffer);
                        std::cout << "Client " << fds[i].fd << ": " << buffer << std::endl;
                        for (size_t j = 0; j < fds.size(); ++j)
                        {
                            if (fds[j].fd != serverfd && fds[j].fd != fds[i].fd)
                            {
                                int sent = send(fds[j].fd, newBuffer.c_str(), newBuffer.length(), 0);
                                if (sent == -1)
                                    std::cout << "Send failed" << std::endl;
                            }
                        }
                    }
                }
            }

        }
    }
    close(serverfd);
    return 0;
    std::cout << "paul" << std::endl;

}