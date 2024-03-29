#include "../includes/server.hpp"

bool Server::_running = true;

Server::Server(int port, std::string password) : _port(port), _password(password), _clients(), pollfds(), _server_socket()
{
    std::cout << "+Server Constructor called" << std::endl;
    _running = true;
}

Server::~Server()
{
    std::map<int, Client*>::iterator itClt;
    for (itClt = _clients.begin(); itClt != _clients.end(); ++itClt)
    {
        delete itClt->second;
        _clients.erase(itClt);
    }
    std::map<std::string, Channel*>::iterator itChannel;
    for (itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel)
    {
        delete itChannel->second;
        _channels.erase(itChannel);
    }
    close(_server_socket);
    std::cout << "Server shutting down" << std::endl;
}

void Server::ServerError(std::string error_str)
{
    error_print(error_str);
    close(_server_socket);
    exit(1);
}

//Creating the listener socket for the server
void Server::ServerListenerSock(void)
{
    int i = 1;
    memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htons(INADDR_ANY);
	server_address.sin_port = htons(getPort());
    
     _server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket < 0)
        ServerError("Server socket creation failed");

    if (fcntl(_server_socket, F_SETFL, O_NONBLOCK) < 0)
		ServerError("Fnctl to nonblocking failed");

    if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR , &i, sizeof(i)) < 0)
		ServerError("SetSock reuse address failed");

	if (setsockopt(_server_socket, IPPROTO_TCP, TCP_NODELAY, &i, sizeof(i)) < 0)
		ServerError("SetSock nodelay");

    if (bind(_server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
	    ServerError("Bind failed");

    if (listen(_server_socket, MAX_USER) < 0)
		ServerError("Listen failed");

    pollfds.push_back((pollfd){_server_socket, POLLIN, 0});
}

int Server::userChecker(Client *clt, std::string user)
{
    std::map<int, Client*>::iterator itClt;

    if (user == clt->getUser())
    {
        error_print("You cant assign the same user");
        return 1;
    }
    for (itClt = _clients.begin(); itClt != _clients.end(); ++itClt)
    {
        if (user == itClt->second->getUser())
        {
            error_print("User already exists");
            // Command::quit(_channels, _clients, pollfds, clt, clt->getCltFd());
            return 1;
        }
    }
    return 0;
}

int Server::nickChecker(Client *clt, std::string nick)
{
    std::map<int, Client*>::iterator itClt;

    if (nick.empty())
    {
        sendIrcMessage(":@localhost 431 " + clt->getNick() + " :Empty nickname", clt->getCltFd());
        return 1;
    }
    if (!checkOneWord(nick) || nick[0] == '#' || nick[0] == ':')
    {
        sendIrcMessage(":@localhost 432 " + clt->getNick() + " :Erroneous nickname", clt->getCltFd());
        return 1;
    }
    if (nick == clt->getNick())
    {
        // clt->setNick(nick);
        sendIrcMessage(":@localhost 433 " + clt->getNick() + " :Nickname in use", clt->getCltFd());
        error_print("You cant assign the same nick");
        return 1;
    }
    for (itClt = _clients.begin(); itClt != _clients.end(); ++itClt)
    {
        if (nick == itClt->second->getNick())
        {
            std::cout << "\n\nnicknames -> " << itClt->second->getNick() << std::endl;
            if (clt->getNick().empty())
                clt->setNick(nick);
            sendIrcMessage(":@localhost 433 " + clt->getNick() + " :Nickname is use", clt->getCltFd());
            error_print("Nick already exists");
            // Command::quit(_channels, _clients, pollfds, clt, clt->getCltFd());
            return 1;
        }
    }
    return 0;
}

// TODO: change func to commands
void Server::executeCmd(Client *clt, std::string cmd, std::string cmdValue)
{
    /*if (cmdValue.empty())
    {
        error_print("No Arguments in cmd");
        return ;
    }*/
    if (cmd.compare("QUIT") == 0)
    {
        Command::quit(_channels, _clients, clt, clt->getCltFd());
    }
    else if (cmd.compare("PRIVMSG") == 0)
    {
        Command::privMsg(_channels, _clients, clt, cmdValue);
    }
    else if (cmd.compare("JOIN") == 0)
    {
        Command::join(_channels, clt, cmdValue);
    }
    else if (cmd.compare("PART") == 0)
    {
        Command::part(_channels, clt, cmdValue);
    }
    else if (cmd.compare("KICK") == 0)
    {
        Command::kick(_channels, clt, cmdValue);
    }
    else if (cmd.compare("TOPIC") == 0)
    {
        Command::topic(_channels, clt, cmdValue, _clients);
    }
    else if (cmd.compare("INVITE") == 0)
    {
        Command::invite(_channels, clt, cmdValue, _clients);
    }
    else if (cmd.compare("MODE") == 0)
    {
        Command::mode(_channels, clt, cmdValue, _clients);
    }
}

void Server::authProcess(Client *clt, char *fullCmd)
{
    if (!clt)
        return ;
    std::string cmd = getCmd(fullCmd);
    std::string cmdValue = getCmdValue(fullCmd);
    if (cmd.compare("PASS") != 0 && !clt->getAuth())
    {
        sendIrcMessage(":@localhost 451 " + clt->getNick() + " :You have not registered" , clt->getCltFd());
        return ;
    }
    if (cmd.compare("PASS") == 0)
    {
        if (clt->getAuth())
            sendIrcMessage(":@localhost 462 " + clt->getNick() + " :Already registered" , clt->getCltFd());
        else
            Command::password(clt, cmdValue, _password);
        }
    else if (cmd.compare("USER") == 0)
    {
        if (clt->getUserDef())
        {
            sendIrcMessage(":@localhost 462 " + clt->getNick() + " :User already registered" , clt->getCltFd());
            return ;
        }
        if (cmdValue.empty())
        {
            sendIrcMessage(":@localhost 461 " + clt->getNick() + " :User already registered" , clt->getCltFd());
            return ;
        }
        if (userChecker(clt, cmdValue) == 0)
            Command::username(clt, cmdValue);
    }
    else if (cmd.compare("NICK") == 0)
    {
        if (nickChecker(clt, cmdValue) == 0)
            Command::nick(_channels, clt, cmdValue);
    }
    else
    {
        if (!clt->getUserDef())
            sendIrcMessage(":@localhost 451 " + clt->getNick() + " :User not registered" , clt->getCltFd());
        else if (!clt->getNickDef())
            sendIrcMessage(":@localhost 431 " + clt->getNick() + " :No nickname given" , clt->getCltFd());
        else
            executeCmd(clt, cmd, cmdValue);
    }
}

void Server::AddClients()
{
    int	addr_len = sizeof(server_address);
	_client_socket = accept(_server_socket, (struct sockaddr*)&server_address, (socklen_t *)&addr_len);
	if (_client_socket < 0)
	{
		error_print("Accept failed");
		close(_client_socket);
		return;
	}
	else
    {
        Client *client = new Client(_client_socket);
        std::cout << "Add client fd -> " << _client_socket << std::endl;
	    _clients[_client_socket] = client;
		pollfds.push_back((pollfd){_client_socket, POLLIN, 0});
    }
}

void Server::parseInitialMsg(int fd, char* fullCmd)
{
    std::istringstream bufferStream(fullCmd);
    std::string line;
    while (std::getline(bufferStream, line)) 
    {
        size_t pos1 = line.find("PASS ");
        if (pos1 != std::string::npos) 
        {
            std::string password1 = line.substr(pos1 + 5);
            password1.erase(password1.size() - 1);
            if (getPassword() != password1)
            {
                sendIrcMessage(":@localhost 464 " + _clients[fd]->getNick() + " :Password incorrect" , _clients[fd]->getCltFd());
                error_print("PASSWORD");
                std::cout << "Client_" << fd << ": auth failed" << std::endl;
                close(fd);
                return ;
            }
            else
            {
                Command::password(_clients[fd], password1, _password);
                success_print("PASSWORD");
            }
        }
        size_t pos2 = line.find("NICK ");
        if (pos2 != std::string::npos) 
        {
            std::string nick1 = line.substr(pos2);
            char* nickBuffer = new char[nick1.length() + 1];
            std::strcpy(nickBuffer, nick1.c_str());
            authProcess(_clients[fd], nickBuffer);
            delete[] nickBuffer;
        }
        // std::cout << "FULLCMD-> " << line << std::endl;
        size_t pos3 = line.find(" 0 * :realname");
        // std::cout << "pos3-> " << pos3 << std::endl;
        if (pos3 != std::string::npos)
        {
            std::string user1 = line.substr(0, pos3);
            // std::cout << "user1-> " << user1 << std::endl;
            char* userBuffer = new char[user1.length() + 1];
            std::strcpy(userBuffer, user1.c_str());
            authProcess(_clients[fd], userBuffer);
            delete[] userBuffer;
        }
    }
}

void Server::availableCMDS()
{
    std::cout << "Nick: /nick nick " << std::endl;
    std::cout << "Join: /join #channel" << std::endl;
    std::cout << "Invite: /invite nick" << std::endl;
    std::cout << "Topic: /topic #channel topic" << std::endl;
    std::cout << "Quit: /quit " << std::endl;
    std::cout << "Mode" << std::endl;
}

void Server::signalHandler(int signal) 
{
    if (signal == SIGINT) 
    {
        std::cout << "Received SIGINT. Stopping server..." << std::endl;
        std::cout << "_running" << _running << std::endl;
        _running = 0;
        std::cout << "_running" << _running << std::endl;
    }
}

int Server::ServerStartUp()
{
    char buf[BUFFER_SIZE];
    char* charPointer;

    signal(SIGINT, signalHandler);
    std::cout << YELLOW << "Server is running on the port: " << RESET << getPort() << std::endl;
    ServerListenerSock();
    availableCMDS();
    while (_running)
	{
		if(0 > poll(pollfds.begin().base(), pollfds.size(), -1))
			std::cout << "poll fail" << std::endl;
		for (size_t i = 0 ; i <  pollfds.size() ; i++)
		{
			if (pollfds[i].revents & POLLHUP)
			{
				std::cout << "Client " << pollfds[i].fd << " disconnected";
                close(pollfds[i].fd);
				break;
			}
			if (pollfds[i].revents & POLLIN)
			{
				if (pollfds[i].fd == pollfds[0].fd)
				{
					AddClients();
					break ;
				}
                std::string message;
                bzero(buf, 100);
                while (!strstr(buf, "\n"))
                {
                    bzero(buf, 100);
                    if ((recv(pollfds[i].fd, buf, 100, 0) <= 0) and (errno != EWOULDBLOCK))
                    {
                        break ;
                    }
                    std::cout << buf << std::endl;
                    message.append(buf);
                }
                charPointer = const_cast<char*>(message.c_str());
                if (pollfds[i].fd != 3)
                {
                    if (((strncmp(buf, "CAP", 3) == 0) || (strncmp(buf, "PASS", 4) == 0 )))
                        parseInitialMsg(pollfds[i].fd, charPointer);
                    else
                    {
                        authProcess(_clients[pollfds[i].fd], charPointer);
                    }
                }
                memset(buf, 0, BUFFER_SIZE);
			}
		}
	}
    return 0;
}

int Server::getPort() const
{
    return _port;
}

std::string const Server::getPassword() const
{
    return _password;
}
    