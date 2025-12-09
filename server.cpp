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
	if (parsed.size() != 2)
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
	if (parsed.size() != 5)
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
	if (parsed.size() != 2)
		std::cout << "Wrong number of arguments" << std::endl;
	if (_clients[fd].isAuthentificated())
		std::cout << "You are already authenticated" << std::endl;
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
	if (parsed.size() > 2)
		std::cout << "Wrong number of arguments" << std::endl;
	else {
		close(fd);
		_clients.erase(fd);
		for (size_t i = 0; i < _fds.size(); ++i)
		{
			if (fd == _fds[i].fd)
			{
				_fds.erase(_fds.begin() + i);
				break;
			}
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

void server::handleJoin(int fd, std::vector<std::string>& parsed)
{
	if (!_clients[fd].isRegistered())
	{
		std::cout << "You need to register before joining a channel" << std::endl;
		return;
	}
	if (parsed.size() < 2)
		std::cout << "Wrong number of arguments" << std::endl;
	else
	{
		char prefix = parsed[1][0];
		if (prefix != '#' && prefix != '&' && prefix != '!' && prefix != '+')
		{
			std::cout << "Invalid channel name : must start with #,&,! or +" << std::endl;
			return;
		}
		if (parsed[1].length() > 50)
		{
			std::cout << "Channel name too long (max 50 characters)" << std::endl;
			return;
		}
		for (size_t i = 1; i < parsed[1].length(); ++i)
		{
			if (!isAuthorisedChar(parsed[1][i]) && parsed[1][i] != '-')
				return;
		}
		if (_channels.find(parsed[1]) != _channels.end())
		{
			_channels[parsed[1]].addMember(fd, &_clients[fd]);
			// return;
		}
		else {
			std::cout << "Creating channel: " << parsed[1] << std::endl;
            std::cout << "Client nickname: " << _clients[fd].getNickname() << std::endl;
			std::cout << "Client username: " << _clients[fd].getUsername() << std::endl;
			channel newChannel(parsed[1], &_clients[fd]);
			_channels[parsed[1]] = newChannel;
			// std::cout << "Channel " << parsed[1] << " created by " << _channels[parsed[1]].getCreator()->getNickname() << std::endl;
		}
		std::string response = ":" + _clients[fd].getNickname() + "!" + 
						_clients[fd].getUsername() + "@localhost JOIN " + 
						parsed[1] + "\r\n";
		send(fd, response.c_str(), response.length(), 0);
		std::string topic = _channels[parsed[1]].getTopic();
		std::cout << "Topic for channel " << parsed[1] << ": " << topic << std::endl;
		response = ":localhost 332 " + _clients[fd].getNickname() + 
				" " + parsed[1] + " :" + topic + "\r\n";
		send(fd, response.c_str(), response.length(), 0);
		std::string namesList;
		std::map<int, clients*> members = _channels[parsed[1]].getMembers();
		std::map<int, clients*>::iterator memberIt;
		for (memberIt = members.begin(); memberIt != members.end(); ++memberIt)
		{
			if (_channels[parsed[1]].isOperator(memberIt->first))
				namesList += "@";
			namesList += memberIt->second->getNickname();
			namesList += " ";
		}
		response = ":" + _clients[fd].getNickname() + "!" + 
								_clients[fd].getUsername() + "@localhost JOIN " + 
								parsed[1] + "\r\n";
			send(fd, response.c_str(), response.length(), 0);
		response = ":localhost 353 " + _clients[fd].getNickname() + 
				" = " + parsed[1] + " :" + namesList + "\r\n";
		send(fd, response.c_str(), response.length(), 0);
		response = ":localhost 366 " + _clients[fd].getNickname() + 
				" " + parsed[1] + " :End of /NAMES list\r\n";
		send(fd, response.c_str(), response.length(), 0);
	}
}

std::string server::_getServerName() const
{
    return _serverName;
}

void server::handleKick(int fd, std::vector<std::string> parsed)
{
	if (parsed.size() < 3)
	{
		std::cout << "Wrong number of arguments" << std::endl;
		return;
	}
	std::string channelName = parsed[1];
	std::string targetNick = parsed[2];

	std::map<std::string, channel>::iterator chanIt = _channels.find(channelName);
	if (chanIt == _channels.end())
	{
		std::cout << "Channel does not exist" << std::endl;
		return;
	}
	if (!chanIt->second.isOperator(fd))
	{
		std::cout << "You are not channel operator" << std::endl;
		return;
	}
	int targetFd = -1;
	std::map<int, clients>::iterator clientIt;
	for (clientIt = _clients.begin(); clientIt != _clients.end(); ++clientIt)
	{
		if (clientIt->second.getNickname() == targetNick)
		{
			targetFd = clientIt->first;
			break;
		}
	}

	if (targetFd == -1)
	{
		std::cout << "User not found" << std::endl;
		return;
	}

	if (!chanIt->second.isMember(targetFd))
	{
		std::cout << "User is not in the channel" << std::endl;
		return;
	}

	chanIt->second.removeMember(targetFd);

	std::string reason = parsed.size() > 3 ? parsed[3] : targetNick;
	std::string response = ":" + _clients[fd].getNickname() + "!" + 
						_clients[fd].getUsername() + "@localhost KICK " + 
						channelName + " " + targetNick + " :" + reason + "\r\n";

	std::map<int, clients*> members = chanIt->second.getMembers();
	for (std::map<int, clients*>::iterator it = members.begin(); it != members.end(); ++it)
	{
		send(it->first, response.c_str(), response.length(), 0);
	}
	send(targetFd, response.c_str(), response.length(), 0);
}

void	server::handleCommands(int fd, std::vector<std::string> parsed) {
	std::string command = parsed[0];

	if (command == "NICK")
		handleNick(fd, parsed);
	else if (command == "USER")
		handleUser(fd, parsed);
	else if (command == "PASS")
		handlePass(fd, parsed);
	else if (command == "PRIVMSG")
		handlePrvMsg(fd, parsed);
	else if (command == "JOIN")
		handleJoin(fd, parsed);
	else if (command == "TOPIC")
		handleTopic(fd, parsed);
	else if (command == "KICK")
		handleKick(fd, parsed);
	else if (command == "QUIT")
		handleQuit(fd, parsed);
	else
		std::cout << "Wrong command" << std::endl;
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

void server::handlePrvMsg(int fd, std::vector<std::string> parsed)
{
	// :nickname!usrname@hostname
	if (!_clients[fd].isRegistered())
	{
		std::cout << "You need to register before joining a channel" << std::endl;
		return;
	}
	if (parsed.size() < 3)
	{
		std::cout << "Wrong number of arguments" << std::endl;
		return;
	}
	std::string prefix;
	prefix = ":" + _clients[fd].getNickname() + "!" + _clients[fd].getUsername() + "@" + _clients[fd].getHostname() + " ";
	std::string target = parsed[1];
	std::string message = prefix + parsed[0] + " " + target + " :" + parsed[2] + "\r\n";
	std::vector<int>targetFds = findTarget(target, fd);
	for (size_t i = 0; i < targetFds.size(); ++i)
	{
		if (targetFds[i] != -1)
		{
			int sent = send(targetFds[i], message.c_str(), message.length(), 0);
			if (sent == -1)
				std::cout << "Send failed" << std::endl;
		}
		else
		{
			std::cout << "No user called like that bro" << std::endl;
		}

	}
}

void server::handleTopic(int fd, std::vector<std::string> parsed)
{
	std::string channelName = parsed[1];
	std::map<std::string, channel>::iterator it = _channels.find(channelName);
	if (it == _channels.end())
	{
		std::cout << "Channel does not exist" << std::endl;
		return;
	}
	if (parsed.size() == 2)
	{
		std::string topic = it->second.getTopic();
		std::string response = ":localhost 332 " + _clients[fd].getNickname() + " " + channelName + " :" + topic + "\r\n";
		send(fd, response.c_str(), response.length(), 0);
	}
	else if (parsed.size() == 3)
	{
		if (!it->second.isOperator(fd))
		{
			std::cout << "You are not channel operator" << std::endl;
			return;
		}
		it->second.setTopic(parsed[2]);
		std::string response = ":" + _clients[fd].getNickname() + "!" + _clients[fd].getUsername() + "@localhost TOPIC " + channelName + " :" + parsed[2] + "\r\n";
		std::map<int, clients*> members = it->second.getMembers();
		for (std::map<int, clients*>::iterator memberIt = members.begin(); memberIt != members.end(); ++memberIt)
		{
			// if (memberIt->first == fd)
			// 	continue;
			int memberFd = memberIt->first;
			send(memberFd, response.c_str(), response.length(), 0);
		}
		// Broadcast to all members
		// For simplicity, we will just send to the client who set the topic
		// send(fd, response.c_str(), response.length(), 0);
	}
	else
	{
		std::cout << "Wrong number of arguments" << std::endl;
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
	
	
	_clients[_fds[i].fd].setBuffer(std::string(buffer));
	std::string msg = _clients[_fds[i].fd].getBuffer();
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
		handleCommands(_fds[i].fd, command);
		_clients[_fds[i].fd].clearBuffer(pos);
		msg = _clients[_fds[i].fd].getBuffer();
		pos = msg.find("\r\n");
	}
	std::cout << _clients[_fds[i].fd] << std::endl;
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
	_serverName = "IRCedric";
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