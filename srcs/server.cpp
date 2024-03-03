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
    if (cmd.compare("/pmsg") == 0)
    {
        Command::pMsg(_clients, clt, cmdValue);
    }
    else if (cmd.compare("/join") == 0)
    {
        Command::join(_channels, clt, cmdValue);
    }
    else if (cmd.compare("/kick") == 0)
    {
        
    }
    else if (cmd.compare("/invite") == 0)
    {

    }
    else if (cmd.compare("/mode") == 0)
    {
        
    }
    else
    {
        error_print("Cmd does not exist");
    }
}

void Server::authProcess(Client *clt, char *fullCmd)
{
    std::string cmd = getCmd(fullCmd);
    std::string cmdValue = getCmdValue(fullCmd);
    if (cmdValue.empty())
    {
        error_print("No Arguments in cmd");
        return ;
    }
    if (!clt->getAuth() && cmd.compare("/pass") == 0)
        Command::password(clt, cmdValue, getPassword());
    else if (!clt->getAuth())
        error_print("Use '/pass [password]' to authenticate");
    else if (clt->getAuth() && cmd.compare("/pass") == 0)
        error_print("Already Authenticated!");
    else
    {
        if (cmd.compare("/user") == 0)
            Command::username(clt, cmdValue);
        else if (cmd.compare("/nick") == 0)
            Command::nick(clt, cmdValue);
        else if (!clt->getNickDef() || !clt->getUserDef())
            error_print("Define username and nickname! Use '/user [username]' and '/nick [nickname]' to define one");
        else
            executeCmd(clt, cmd, cmdValue);
    }
}

void Server::History()
{
    //SERVER CHAT HISTORY
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
    // int client_id = 1;

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
        //IPv4 convertion
        //inet_ntop(AF_INET, &(clienteAddr.sin_addr), remoteIP, INET_ADDRSTRLEN);
        printf("Server: New Client connection from %s client id: %d\n", remoteIP, fd_count);
    }
}

//Creating the listener socket for the server
int Server::ServerListenerSock(void)
{
    int _server_socket;

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

    ServerIsRunning();
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
                    int sender_fd = pfds[i].fd;
                    if (nbytes <= 0)
                    {
                        if (nbytes == 0) 
                            printf("SERVER: socket %d hung up\n", sender_fd);
                        close(pfds[i].fd); // CLOSE CLIENT
                        del_from_pfds(pfds, i, &fd_count);
                    }
                    else 
                    {
                        std::cout << "Sending data from the client_fd:" << sender_fd << std::endl;
                        // We got data from a client
                        for(int j = 0; j < fd_count; j++) 
                        {
                            int dest_fd = pfds[j].fd;
                            // Sending except to SERVER and CURRENT CLIENT_SERVER
                            if (dest_fd != _server_listener && dest_fd != sender_fd) 
                            {
                                std::cout << "to the client_fd" << dest_fd << std::endl;
                                if (Check_if_buf_cmd(buf) == 0)
                                    authProcess(_clients[sender_fd], buf);
                                else if (send(dest_fd, buf, nbytes, 0) < 0) 
                                    error_print("Send failed");
                            }
                            //for server + 1 client
                            else if (fd_count == 2)
                            {
                                std::cout << "to the client_fd" << dest_fd << std::endl;
                                if (Check_if_buf_cmd(buf) == 0)
                                    authProcess(_clients[sender_fd], buf);
                                break ;
                            }
                        }
                        memset(buf, 0, sizeof(buf));
                    }
                }
            }
        }
    }
    
    return 0;
}

void Server::ServerIsRunning()
{
    std::cout << YELLOW << "Server Port: " << RESET << getPort() << std::endl;
    std::cout << YELLOW << "Server Password: " << RESET << getPassword() << std::endl;
}

int Server::getPort() const
{
    return _port;
}

std::string const Server::getPassword() const
{
    return _password;
}
    