#include "../includes/server.hpp"

Server::Server(int port, std::string password) : _port(port), _password(password), _clients(), pollfds(), _server_socket()
{
    std::cout << "+Server Constructor called" << std::endl;
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

void Server::nickChecker(Client *clt, std::string nick)
{
    std::map<int, Client*>::iterator itClt;
    for (itClt = _clients.begin(); itClt != _clients.end(); ++itClt)
    {
        if (nick == clt->getNick() && _clients.size() > 1)
        {
            std::cout << "NICK IS THE SAME" << std::endl;
            Command::quit(_clients, pollfds, clt, clt->getCltFd());
        }
    }
    std::cout << "NICK IS NOT THE SAME" << std::endl;
}

// TODO: change func to commands
void Server::executeCmd(Client *clt, std::string cmd, std::string cmdValue)
{
    std::cout << "Client_" << clt->getCltFd() << "Executing cmd" << std::endl;
    if (cmd.compare("QUIT") == 0)
    {
        Command::quit(_clients, pollfds, clt, clt->getCltFd());
    }
    else if (cmd.compare("PRIVMSG") == 0)
    {
        Command::privMsg(_channels, _clients, clt, cmdValue);
    }
    else if (cmd.compare("JOIN") == 0)
    {
        Command::join(_channels, clt, cmdValue);
    }
    else if (cmd.compare("KICK") == 0)
    {

    }
    else if (cmd.compare("INVITE") == 0)
    {

    }
    else if (cmd.compare("MODE") == 0)
    {

    }
    else
    {
        error_print("Cmd does not exist");
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
		this->pollfds.push_back((pollfd){_client_socket, POLLIN, 0});
    }
}

void Server::authProcess(Client *clt, int fd, char *fullCmd)
{
    std::string cmd = getCmd(fullCmd);
    std::string cmdValue = getCmdValue(fullCmd);
    std::cout << "cmd ->" << cmd << ". | cmdValue ->" << cmdValue << ".\n";
    if (cmdValue.empty())
    {
        error_print("No Arguments in cmd");
        return ;
    }
    if (cmd.compare("USER") == 0)
        Command::username(clt, cmdValue);
    else if (cmd.compare("NICK") == 0)
    {
        nickChecker(clt, cmdValue);
        Command::nick(clt, cmdValue);
    }
    else
        executeCmd(clt, cmd, cmdValue);
}

void Server::parseInitialMsg(Client *clt, int fd, char* fullCmd)
{
    std::istringstream bufferStream(fullCmd);
    std::string line;
    //std::cout << "\n\n------" << bufferStream << "\n\n-----------" << std::endl;
    while (std::getline(bufferStream, line)) 
    {
        size_t pos1 = line.find("PASS ");
        if (pos1 != std::string::npos) 
        {
            std::string password1 = line.substr(pos1 + 5);
            password1.erase(password1.size() - 1);
            if (getPassword() != password1)
            {
                error_print("PASSWORD");
                std::cout << "Client_" << fd << ": auth failed" << std::endl;
                close(fd);
                return ;
            }
            else
            {
                success_print("PASSWORD");
            }
        }
        size_t pos2 = line.find("NICK ");
        if (pos2 != std::string::npos) 
        {
            std::string nick1 = line.substr(pos2);
            char* nickBuffer = new char[nick1.length() + 1];
            std::strcpy(nickBuffer, nick1.c_str());
            authProcess(_clients[fd], fd, nickBuffer);
            delete[] nickBuffer;
        }
        size_t pos3 = line.find(" 0 * :realname");
        if (pos3 != std::string::npos)
        {
            std::string user1 = line.substr(0, pos3);
            char* userBuffer = new char[user1.length() + 1];
            std::strcpy(userBuffer, user1.c_str());
            authProcess(_clients[fd], fd, userBuffer);
            delete[] userBuffer;
        }
    }
}

int Server::ServerStartUp()
{
    char buf[BUFFER_SIZE];

    std::cout << YELLOW << "Server is running on the port: " << RESET << getPort() << std::endl;
    ServerListenerSock();
    while (1)
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
	            if(recv(pollfds[i].fd, buf, BUFFER_SIZE, 0) == -1)
	            {
	            	std::cerr << "Buffer Error" << std::endl;
	            }
                std::cout << "\nBuffer:" << buf << "\n" << std::endl;
                if (pollfds[i].fd != 3)
                {
                    if (((strncmp(buf, "CAP", 3) == 0) || (strncmp(buf, "PASS", 4) == 0 )) && pollfds[i].fd != 3)
                        parseInitialMsg(_clients[pollfds[i].fd], pollfds[i].fd, buf);
                    else
                    {
                        std::cout << "EXECUTE CMD NOT INITIAL" << std::endl;
                        authProcess(_clients[pollfds[i].fd], pollfds[i].fd, buf);
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
    