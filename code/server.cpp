#include "server.hpp"
#include "command.hpp"
#include "commands/nickCommand.hpp"
#include "commands/userCommand.hpp"
#include "commands/passCommand.hpp"
#include "commands/joinCommand.hpp"
#include "commands/privmsgCommand.hpp"
#include "commands/topicCommand.hpp"
#include "commands/kickCommand.hpp"
#include "commands/quitCommand.hpp"
#include "commands/modeCommand.hpp"
#include "commands/inviteCommand.hpp"
#include "commands/whoCommand.hpp"
#include "commands/capCommand.hpp"

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
		std::string notice = "NOTICE AUTH :*** Looking up your hostname...\r\n";
		send(clientSocket, notice.c_str(), notice.length(), 0);
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
	else
		command = buffer;
	std::transform(command.begin(), command.end(), command.begin(), ::toupper);
	parsed.push_back(command);
	pos = params.find(':');
	if (pos != std::string::npos) {
		trail = params.substr(pos + 1);
		middle = params.substr(0, pos);
	}
	else
		middle = params;
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

std::string server::getPassword() const {
	return _password;
}

clients& server::getClient(int fd) {
	return _clients[fd];
}

std::map<std::string, channel>& server::getChannels() {
	return _channels;
}

std::map<int, clients>& server::getClients() {
	return _clients;
}

void server::removeClient(int fd) {
	_clients.erase(fd);
	for (size_t i = 0; i < _fds.size(); ++i) {
		if (fd == _fds[i].fd) {
			_fds.erase(_fds.begin() + i);
			break;
		}
	}
}

void printParsed(const std::vector<std::string>& parsed)
{
	std::cout << "Parsed command:" << std::endl;
	for (size_t i = 0; i < parsed.size(); ++i)
	{
		std::cout << "Part " << i << ": " << parsed[i] << std::endl;
	}
}

std::string server::_getServerName() const
{
	return _serverName;
}

// void server::registerCommand(const std::string& name, const command& cmd) {
// 	commandMap[name] = cmd;
// }

void server::registerCommands() {
	_commands["NICK"] = new NickCommand(this);
	_commands["USER"] = new UserCommand(this);
	_commands["PASS"] = new PassCommand(this);
	_commands["JOIN"] = new JoinCommand(this);
	_commands["PRIVMSG"] = new PrivmsgCommand(this);
	_commands["TOPIC"] = new TopicCommand(this);
	_commands["KICK"] = new KickCommand(this);
	_commands["QUIT"] = new QuitCommand(this);
	_commands["MODE"] = new ModeCommand(this);
	_commands["INVITE"] = new InviteCommand(this);
	_commands["WHO"] = new WhoCommand(this);
	_commands["CAP"] = new CapCommand(this);
}

void	server::handleCommands(int fd, const std::vector<std::string>& parsed) {
	std::string command = parsed[0];

	std::map<std::string, Command*>::iterator it = _commands.find(command);
	if (it != _commands.end()) {
		it->second->execute(fd, parsed);
	} else {
		std::cout << "Wrong command" << std::endl;
	}
}

int server::findClientByNickname(const std::string& nickname) {
	for (std::map<int, clients>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (it->second.getNickname() == nickname) {
			return it->first;
		}
	}
	return -1;
}

std::vector<int> server::findTarget(std::string target, int senderFd)
{
	std::vector<int> targetFds;

	std::stringstream ss(target);
	std::string singleTarget;
	
	while (std::getline(ss, singleTarget, ','))
	{
		bool found = false;
		if (!singleTarget.empty() && (singleTarget[0] == '#' || singleTarget[0] == '&' || 
									   singleTarget[0] == '!' || singleTarget[0] == '+'))
		{
			std::map<std::string, channel>::iterator chanIt = _channels.find(singleTarget);
			if (chanIt != _channels.end())
			{
				std::map<int, clients*> members = chanIt->second.getMembers();
				std::map<int, clients*>::iterator memberIt;
				for (memberIt = members.begin(); memberIt != members.end(); ++memberIt)
				{
					if (memberIt->first != senderFd)
						targetFds.push_back(memberIt->first);
				}
				found = true;
			}
		}
		else {
			std::map<int, clients>::iterator it;
			for (it = _clients.begin(); it != _clients.end(); it++)
			{
				if (singleTarget == it->second.getNickname())
				{
					targetFds.push_back(it->first);
					found = true;
					break;
				}
			}
		}
		if (!found)
			targetFds.push_back(-1);
	}
	return targetFds;
}

void server::handleClientMessage(size_t& i)
{
	char buffer[1024];
	int n = recv(_fds[i].fd, buffer, sizeof(buffer) - 1, 0);
	int currentFd = _fds[i].fd;
	if (n <= 0)
	{
		_clients.erase(currentFd);
		disconnectClient(_fds, i);
		return;
	} 
	buffer[n] = '\0';
	
	_clients[currentFd].setBuffer(std::string(buffer));
	std::string msg = _clients[currentFd].getBuffer();
	size_t pos = msg.find("\r\n");
	std::cout << "pos: " << pos << std::endl;
	if (pos == std::string::npos) {
		return;
	}
	while (pos != std::string::npos)
	{
		std::string fullMess = msg.substr(0, pos);
		std::vector<std::string> command = parseCommand(fullMess);
		printParsed(command);
		handleCommands(currentFd, command);
		if (command[0] == "QUIT") {
            // Client was removed, decrement i to adjust for removed fd
            i--;
			return;
        }
		_clients[currentFd].clearBuffer(pos);
		msg = _clients[currentFd].getBuffer();
		pos = msg.find("\r\n");
	}
	std::cout << _clients[currentFd] << std::endl;
	// std::stringstream ss;
	// ss << _fds[i].fd;
	// std::string clientName = "Client " + ss.str() + ": ";
	// std::string newBuffer = clientName + std::string(buffer);
	// std::cout << newBuffer << std::endl;
	// broadcastMessage(_fds[i].fd, newBuffer);
}

channel *server::getChannel(const std::string &name)
{
	std::map<std::string, channel>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return &(it->second);
	return NULL;
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
	_serverName = "IRCedric";
	_password = password;
	registerCommands();
}

server::~server()
{
	for (std::map<std::string, Command*>::iterator it = _commands.begin(); 
		 it != _commands.end(); ++it) {
		delete it->second;
	}
	_commands.clear();
	
	close(_serverfd);
	for (size_t i = 0; i < _fds.size(); ++i)
	{
		close(_fds[i].fd);
	}
}