#include "server.hpp"

pollfd    createPoll(int fd)
{
	pollfd newPollFd;
	newPollFd.fd = fd;
	newPollFd.events = POLLIN;
	return newPollFd;
}

void    server::connectClient()
{
	int clientSocket = accept(_serverfd, NULL, NULL);
	if (clientSocket != -1)
	{
		pollfd clientPollFd = createPoll(clientSocket);
		_fds.push_back(clientPollFd);
		std::cout << "New client connected: " << clientSocket << std::endl;
	}
}

void disconnectClient(std::vector<pollfd>& fds, size_t& i)
{
	std::cout << "Client disconnected: " << fds[i].fd << std::endl;
	close(fds[i].fd);
	fds.erase(fds.begin() + i);
	i--;
}

void server::broadcastMessage(int senderfd, const std::string& message)
{
	for (size_t j = 0; j < _fds.size(); ++j)
	{
		if (_fds[j].fd != _serverfd && _fds[j].fd != senderfd)
		{
			int sent = send(_fds[j].fd, message.c_str(), message.length(), 0);
			if (sent == -1)
				std::cout << "Send failed" << std::endl;
		}
	}
}

void server::handleClientMessage(size_t& i)
{
	char buffer[1024];
	int n = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
	if (n <= 0)
	{
		disconnectClient(_fds, i);
		return;
	} 
	buffer[n] = '\0';
	std::stringstream ss;
	ss << _fds[i].fd;
	std::string clientName = "Client " + ss.str() + ": ";
	std::string newBuffer = clientName + std::string(buffer);
	std::cout << newBuffer << std::endl;
	broadcastMessage(_fds[i].fd, newBuffer);
}

void server::initServer()
{
    _serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverfd == -1)
    {
        std::cerr << "Socket creation failed" << std::endl;
        throw std::runtime_error("Socket creation failed");
    }
    
    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(_port);
    
    if (bind(_serverfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Bind failed" << std::endl;
        close(_serverfd);
        throw std::runtime_error("Bind failed");
    }
    
    if (listen(_serverfd, SOMAXCONN) == -1)
    {
        std::cerr << "Listen failed" << std::endl;
        close(_serverfd);
        throw std::runtime_error("Listen failed");
    }
    
    pollfd serverPollFd;
    serverPollFd.fd = _serverfd;
    serverPollFd.events = POLLIN;
    _fds.push_back(serverPollFd);
}

void server::runServer()
{
    while (1)
    {
        if (poll(_fds.data(), _fds.size(), -1) < 0)
        {
            std::cerr << "Poll error" << std::endl;
            break;
        }
        for (size_t i = 0; i < _fds.size(); ++i)
        {
            if (_fds[i].revents & POLLIN)
            {
                if (_fds[i].fd == _serverfd)
                    connectClient();
                else
                    handleClientMessage(i);
            }
        }
    }
}

server::server(int port)
{
    _serverfd = -1;
    _port = port;
}

server::~server()
{
    close(_serverfd);
    for (size_t i = 0; i < _fds.size(); ++i)
    {
        close(_fds[i].fd);
    }
}