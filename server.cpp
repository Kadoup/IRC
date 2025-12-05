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
		clients paul(clientSocket);
		_clients[clientSocket] = paul;
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

bool isAuthorisedChar(char c)
{
	if (!std::isalnum(c) &&
			c != '[' && c != ']' && c != '\\' && 
			c != '`' && c != '_' && c != '^' && 
			c != '{' && c != '|' && c != '}')
		return false;
	return true;
}

bool isValidNickname(const std::string& param)
{
	if (param.empty() || param.length() > 9)
		return false;
	char first = param[0];
	if(!isAuthorisedChar(first))
		 return false;
	for(size_t i = 1; i < param.length(); ++i)
	{
		if (!isAuthorisedChar(param[i]) && param[i] != '-')
			return false;
	}
	return true;
}

bool    checkParams(std::string params, std::string command)
{
	if (command == "NICK")
	{
		size_t pos = params.find(' ');
		if (pos == std::string::npos)
			return false;
		if (!isValidNickname(params))
			return false;
		return true;
	}
	else if (command == "PASS")
	{
		size_t pos = params.find(' ');
		if (pos == std::string::npos)
			return false;
		return true;
	}
}


std::vector<std::string> parseCommand(std::string buffer)
{
	std::vector<std::string> parsed;
	std::string command;
	std::string params;
	std::string middle;
	std::string trail;
	
	size_t pos = buffer.find(' ');
	if (pos != std::string::npos) {
		command = buffer.substr(0, pos);
		params = buffer.substr(pos + 1);
	}
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);
	parsed.push_back(command);
	pos = params.find(':');
	if (pos != std::string::npos) {
		trail = params.substr(pos + 1);
		middle = params.substr(0, pos);
	}
	else {
		middle = params;
	}
	std::stringstream ss(middle);
	std::string arg;
	while (ss >> arg) {
		parsed.push_back(arg);
	}
	if (!trail.empty()) {
		parsed.push_back(trail);
	}
	return parsed;
}

void    server::userAuthentification(int fd, std::string buffer)
{
	std::string command;
	std::string params;
	size_t pos = buffer.find(' ');
	if (pos != std::string::npos) {
		command = buffer.substr(0, pos);
		params = buffer.substr(pos + 1);
	}
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);
	if (command == "NICK")
		_clients[fd].setNickname("blou");
	else if (command == "USER")
		_clients[fd].setUsername("blou");
	else if (command == "PASS")
	{
		if("blou" == _password)
		{
			std::cout << "Welcome " << _clients[fd].getNickname() << std::endl;
			_clients[fd].setAuthentificated(true);
		}
		else
			std::cout << "Wrong Password" << std::endl;
	}
	else
		std::cout << "Wrong command" << std::endl;
}

bool server::isUniqueNickname(std::string nick)
{
	std::map<int, clients>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); it++)
	{
		if (nick == it->second.getNickname())
			return false;
	}
	return true;
}

void	server::handleNick(int fd, std::vector<std::string> parsed)
{
	if (parsed.size() > 2)
		std::cout << "Wrong number of arguments" << std::endl;
	else {
		if (!isValidNickname(parsed[1]))
			std::cout << "Bad nickname" << std::endl;
		else if (!isUniqueNickname(parsed[1]))
			std::cout << "Nickname is not unique" << std::endl;
		else {
			_clients[fd].setNickname(parsed[1]);
		}
	}
}

void server::handleUser(int fd, std::vector<std::string> parsed)
{
	if (parsed.size() != 4)
		std::cout << "Wrong number of arguments" << std::endl;
	else {
		if (!isValidNickname(parsed[1]))
			std::cout << "Bad Username" << std::endl;
		else {
			_clients[fd].setUsername(parsed[1]);
		}
		_clients[fd].setRealName(parsed[4]);
	}
}

void server::handlePass(int fd, std::vector<std::string> parsed)
{
	if (parsed.size() > 2)
		std::cout << "Wrong number of arguments" << std::endl;
	else 
	{
		if (parsed[1] == _password)
		{
			std::cout << "Welcome " << _clients[fd].getNickname() << std::endl;
			_clients[fd].setAuthentificated(true);
		}
		else 
			std::cout << "Wrong Password" << std::endl;
	}
}

void server::handleQuit(int fd, std::vector<std::string> parsed)
{
	(void)parsed;
	if (parsed.size() > 2)
		std::cout << "Wrong number of arguments" << std::endl;

	std::cout << "Client disconnected: " << fd << std::endl;

	close(fd);
	_clients.erase(fd);
	
	for (size_t i = 0; i < _fds.size(); ++i)
	{
		if (_fds[i].fd == fd)
		{
			_fds.erase(_fds.begin() + i);
			break;
		}
	}
}

void	server::handleCommands(int fd, std::vector<std::string> parsed) {
	std::string command = parsed[0];

	if (command == "NICK")
		handleNick(fd, parsed);
	else if (command == "USER")
		handleUser(fd, parsed);
	else if (command == "PASS")
		handlePass(fd, parsed);
	else if (command == "QUIT")
		handleQuit(fd, parsed);
	else
		std::cout << "Wrong command" << std::endl;
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
	
	std::vector<std::string> command = parseCommand(std::string(buffer));
	handleCommands(_fds[i].fd, command);
	userAuthentification(_fds[i].fd, std::string(buffer));
	// std::stringstream ss;
	// ss << _fds[i].fd;
	// std::string clientName = "Client " + ss.str() + ": ";
	// std::string newBuffer = clientName + std::string(buffer);
	// std::cout << newBuffer << std::endl;
	// broadcastMessage(_fds[i].fd, newBuffer);
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

server::server(int port, std::string password)
{
	_serverfd = -1;
	_port = port;
	_password = password;
}

server::~server()
{
	close(_serverfd);
	for (size_t i = 0; i < _fds.size(); ++i)
	{
		close(_fds[i].fd);
	}
}