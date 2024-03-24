#include "../includes/server.hpp"

Server::Server(int port, std::string password) : _port(port), _password(password), _clients(), pfds(), _server_socket(), _server_listener()
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
    close(_server_listener);
    std::cout << "Server shutting down" << std::endl;
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

void Server::authProcess(Client *clt, char *fullCmd)
{
    if (clt->getFirstAuth() == 1)
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
    }
}

void Server::ServerError(std::string error_str)
{
    error_print(error_str);
    isRunning = false;
}

// Add a new file descriptor to the poll set
void Server::add_to_pfds(struct pollfd *pfds[], int client_fd, int *fd_count, int *fd_size)
{
    if (*fd_count == *fd_size) 
    {
        ServerError("Maximum clients reached");
        exit(1);
    }
    (*pfds)[*fd_count].fd = client_fd;
    (*pfds)[*fd_count].events = POLLIN; // ready-to-read
    (*fd_count)++;
}

void Server::del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

void Server::AddClients(int &fd_count, int &MAX_FDS)
{
    struct sockaddr_in clienteAddr;
    socklen_t addrlen;
    char remoteIP[INET_ADDRSTRLEN];
    int client_fd = 0;

    // Handle new connection
    addrlen = sizeof clienteAddr;
    client_fd = accept(_server_listener, (struct sockaddr *)&clienteAddr, &addrlen);
    if (client_fd == -1) 
        error_print("Accept");
    else
    {
        //ADD CLIENTS
        Client *client = new Client(client_fd, clienteAddr);
        std::cout << "add client fd -> " << client_fd << std::endl;
	    _clients[client_fd] = client;
        std::cout << "CLIENT_FD 1:" << client_fd << std::endl;
        add_to_pfds(&pfds, client_fd, &fd_count, &MAX_FDS);
        inet_ntop(AF_INET, &(clienteAddr.sin_addr), remoteIP, INET_ADDRSTRLEN);
        printf("Server: New Client connection from %s client id: %d\n", remoteIP, fd_count);
    }
}

//Creating the listener socket for the server
int Server::ServerListenerSock(void)
{
    struct sockaddr_in server_address;
     _server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_socket < 0)
        error_print("Server socket creation failed");
    memset(&server_address, 0, sizeof(server_address));
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htons(INADDR_ANY);
	server_address.sin_port = htons(getPort());
    
    if (bind(_server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
	    error_print("Bind failed");
    if (listen(_server_socket, server_address.sin_port) < 0)
		error_print("Listen failed");

    if (_server_socket == -1) 
        ServerError("Socket Failted");
    return _server_socket;
}

int Server::Check_if_buf_cmd(char *buf)
{
    if (strncmp(buf, "/", 1) == 0)
    {
        return 0;
    }
    return 1;
}

int Server::ServerStartUp()
{
    char buf[BUFFER_SIZE];
    int MAX_FDS = FDS_SIZE;
    int fd_count = 1;

    // Set up the server and listen_socket
    _server_listener = ServerListenerSock();
    pfds = (struct pollfd*)malloc((sizeof *pfds) * (MAX_FDS));
    pfds[0].fd = _server_listener;
    pfds[0].events = POLLIN; // Report ready to read on incoming connection

    std::cout << YELLOW << "Server is running on the port: " << RESET << getPort() << std::endl;
    while (isRunning == true)
    {
        int poll_count = poll(pfds, fd_count, -1);
        if (poll_count == -1)
            ServerError("Poll failed");
        // Run through the existing connections looking for data to read
        for(int i = 0; i < fd_count; i++) 
        {
            // Check if someone's ready to read
            if (pfds[i].revents & POLLIN) 
            {
                if (pfds[i].fd == _server_listener) 
                {
                    AddClients(fd_count, MAX_FDS);
                }
                else
                {
                    // If not the _server_listener, we're just a regular client
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);
                    buf[nbytes] = '\0';
                    _sender_fd = pfds[i].fd;
                    if (nbytes <= 0)
                    {
                        if (nbytes == 0) 
                            printf("SERVER: socket %d hung up\n", _sender_fd);
                        close(pfds[i].fd); // CLOSE CLIENT
                        del_from_pfds(pfds, i, &fd_count);
                    }
                    else
                    {
                        std::cout << "Client " << _sender_fd << ": sending cmd" << std::endl;
                        authProcess(_clients[_sender_fd], buf);
                    }
                    memset(buf, 0, sizeof(buf));
                }
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
    