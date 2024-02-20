#include "server.hpp"

Server::Server(int port, std::string password) : _port(port), _password(password), _clients()
{
    std::cout << "+Server Constructor called" << std::endl;
}

Server::~Server()
{
    std::map<int, Client*>::iterator it;
    for (it = _clients.begin(); it != _clients.end(); ++it)
    {
        delete it->second;
    }
    std::cout << "-Server Destructor called" << std::endl;
}

int Server::getPort() const
{
    return _port;
}

std::string const Server::getPassword() const
{
    return _password;
}

void Server::checkCmd(Client *clt, char *cmd)
{
    if (strncmp(cmd, "/pass ", 5) == 0) 
    {
        clt->cmdPassword(cmd, getPassword());
    }
    else if (strncmp(cmd, "/username ", 9) == 0) 
    {
        clt->cmdNick(cmd);
    }
    else if (strncmp(cmd, "/nick ", 5) == 0) 
    {
        clt->cmdNick(cmd);
    }
    else
    {
        error_print("Command does not exist");
    }
}

//https://www.cs.toronto.edu/~krueger/csc209h/f05/lectures/Week11-Select.pdf
//IO nonblocking server

int Server::ServerStartUp()
{
    std::cout << "Server Port: " << getPort() << std::endl;
    std::cout << "Server Password: " << getPassword() << std::endl;

    signal_handlers();

    int clientSockets[MAX];
    int next_id = 0;
    char buffer[BUFFER_SIZE];

    // Step 1: Create the server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) 
    {
        error_print("Socket creation failed");
        return 1;
    }

    // Step 2: Bind the socket to a specific address and port
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(getPort());
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == 1) 
    {
        error_print("Bind Failed");
        close(server_socket);
        return 1;
    }

    // Step 3: Listen for incoming connections
    if (listen(server_socket, MAX) == -1) 
    {
        error_print("Listen failed");
        close(server_socket);
        return 1;
    }
    std::cout << "Server listening on port " << getPort() << std::endl;

    // Set up pollfd structure
    // STILL NEEDS IMPROVEMENTS WITH THE CLIENT CLASS
    struct pollfd fds[MAX + 1];  // +1 for the server_socket
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    int maxSocket = server_socket;
    for (int i = 1; i <= MAX; i++)
        fds[i].fd = -1;  // Creating the clients FDS

    // Main server loop
    while (isRunning == true) 
    {
        // Step 4: Use poll to wait for events
        int poll_result = poll(fds, maxSocket + 1, -1);
        if (poll_result == -1) 
        {
            error_print("Poll failed");
            setIsRunning(false);
        }
        if (poll_result > 0) 
        {
            if (fds[0].revents & POLLIN) 
            {
                // Step 5: Accept a connection
                int client_socket = accept(server_socket, NULL, NULL);
                if (client_socket == -1)
                {
                    error_print("Accept failed");
                    setIsRunning(false);
                }
                // We will use this client class when the default int clientSockets[MAX]/client_socket works correctly, just testing right now
                _clients[client_socket] = new Client(client_socket);

                // Add the new client socket (IN THE FUTURE CLASS CLIENT)
                for (int i = 1; i <= MAX; i++) 
                {
                    if (fds[i].fd == -1) 
                    {
                        fds[i].fd = client_socket;
                        fds[i].events = POLLIN;
                        break;
                    }
                }
                if (client_socket > maxSocket)
                    maxSocket = client_socket;
                // Step 6: Read and write data
                sprintf(buffer, "server: client %d just arrived\n", next_id);
                send(client_socket, buffer, strlen(buffer), 0);
                // Store the client socket for future references
                clientSockets[next_id++] = client_socket;

                // Check each client socket for activity
                for (int i = 1; i <= MAX; i++) 
                {
                    if (fds[i].revents & POLLIN) 
                    {
                        int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);
                        if (bytesRead <= 0) 
                        {
                            // Client disconnected
                            sprintf(buffer, "server: client %d just left\n", i);

                            // Notify clients about the disconnected client
                            for (int j = 1; j <= MAX; j++) 
                            {
                                if (fds[j].fd != -1 && fds[j].fd != fds[i].fd) 
                                    send(fds[j].fd, buffer, strlen(buffer), 0);
                            }
                            // Close the socket and remove it from the FD poll array
                            close(fds[i].fd);
                            fds[i].fd = -1;
                        } 
                    else
                    {
                        // check for the cmd from the clt
                        if (_clients.find(client_socket) != _clients.end())
                            checkCmd(_clients[client_socket], buffer);
    
                        // Broadcast the message to the other clients
                        buffer[bytesRead] = '\0';
                        char serverBuffer[BUFFER_SIZE];
                        sprintf(serverBuffer, "client %d: %s\n", i, buffer);
                        for (int j = 1; j <= MAX; j++) 
                        {
                            if (fds[j].fd != -1 && fds[j].fd != fds[i].fd) 
                                send(fds[j].fd, serverBuffer, strlen(serverBuffer), 0);
                        }
                    }
                    }
                }
            }
        }
    }
    // step 7: close the server/client sockets
    for (int i = 0; i < next_id; i++) 
        close(clientSockets[i]);
    close(server_socket);
    return 0;
}
