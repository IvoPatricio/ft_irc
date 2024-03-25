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
    }
    std::map<std::string, Channel*>::iterator itChannel;
    for (itChannel = _channels.begin(); itChannel != _channels.end(); ++itChannel)
    {
        delete itChannel->second;
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

// TODO: change func to commands
void Server::executeCmd(Client *clt, std::string cmd, std::string cmdValue)
{
    std::cout << "execute cmd" << std::endl;
    if (cmd.compare("PRIVMSG") == 0)
    {
        Command::privMsg(_clients, clt, cmdValue);
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
		this->pollfds.push_back((pollfd){_client_socket, POLLIN, 0});
}

void Server::authProcess(int fd)
{
    /*if (clt->getFirstAuth() == 1)
    {
        std::string user;
        std::string nick;
        char* CmdCopy = strdup(fullCmd);

        if (CmdCopy == NULL) 
        {
            std::cerr << "Memory allocation failed." << std::endl;
            return ;
        }
        char* token = std::strtok(CmdCopy, "\n");
        while (token != NULL) 
        {
            char command[100];
            char value[100];
            sscanf(token, "%s %s", command, value);
            std::string str(value);
            if (strcmp(command, "USER") == 0)
                user.assign(str);
            else if (strcmp(command, "NICK") == 0)
                nick.assign(str);
            else if (!clt->getAuth() && strcmp(command, "PASS") == 0)
                Command::password(clt, str, getPassword());
            token = std::strtok(NULL, "\n");
        }
        Command::username(clt, user);
        Command::nick(clt, nick);
        free(CmdCopy);
        clt->setFirstAuth(2);
    }
    else
    {
        std::string cmd = getCmd(fullCmd);
        std::string cmdValue = getCmdValue(fullCmd);
        std::cout << "cmd ->" << cmd << ". | cmdValue ->" << cmdValue << ".\n";
        if (cmdValue.empty())
        {
            error_print("No Arguments in cmd");
            return ;
        }
        if (!clt->getAuth() && cmd.compare("PASS") == 0)
            Command::password(clt, cmdValue, getPassword());
        else if (!clt->getAuth())
            error_print("Use '/PASS [password]' to authenticate");
        else if (clt->getAuth() && cmd.compare("/PASS") == 0)
            error_print("Already Authenticated!");
        else
        {
            if (cmd.compare("USER") == 0)
                Command::username(clt, cmdValue);
            else if (cmd.compare("NICK") == 0)
                Command::nick(clt, cmdValue);
            else
                executeCmd(clt, cmd, cmdValue);
        }
    }*/
}

int Server::ServerStartUp()
{
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
				std::cout << "EXECUTE CMD" << std::endl;
				authProcess(pollfds[i].fd);
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
    