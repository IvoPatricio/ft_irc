#include "../includes/server.hpp"

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

int Server::ServerStartUp()
{
    std::cout << "Server Port: " << getPort() << std::endl;
    std::cout << "Server Password: " << getPassword() << std::endl;

    signal_handlers();

    int client_sockets[MAX_FDS];
    int client_id = 0;
    char buffer[BUFFER_SIZE];

    // Step 1: Create the server socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) 
    {
        error_print("Socket creation failed");
        return 1;
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(getPort());

    // Step 2: Bind the socket to a specific address and port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) 
    {
        error_print("Bind Failed");
        close(server_socket);
        return 1;
    }

    // Step 3: Listen for incoming connections
    if (listen(server_socket, MAX_FDS) < 0) 
    {
        error_print("Listen failed");
        close(server_socket);
        return 1;
    }

    struct pollfd fds[MAX_FDS + 1];  // +1 for the server socket
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    for (int i = 1; i <= MAX_FDS; i++) 
        fds[i].fd = -1;  // Initialize client sockets to -1
    int maxSocket = server_socket;

    // Step 4: Use poll to wait for events
    while (isRunning == true) 
    {
        int poll_result = poll(fds, maxSocket + 1, -1);
        if (poll_result < 0) 
        {
            error_print("Poll failed");
            setIsRunning(false);
        }

        // Check server socket for new connection
        if (fds[0].revents & POLLIN) 
        {
            // Step 5: Accept a connection
            int client_socket = accept(server_socket, NULL, NULL);
            if (client_socket < 0) 
            {
                error_print("Accept failed");
                setIsRunning(false);
            }

            // We will use this client class when the default int clientSockets[MAX]/client_socket works correctly, just testing right now
            _clients[client_socket] = new Client(client_socket);

            // Add the new client socket (IN THE FUTURE CLASS CLIENT)
            for (int i = 1; i <= MAX_FDS; i++) 
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

            // Send a welcome message to the client
            sprintf(buffer, "server: client %d just arrived\n", client_id + 1);
            send(client_socket, buffer, strlen(buffer), 0);
            // Store the client socket for future references
            client_sockets[client_id] = client_socket;
            client_id++;
        }

        // Check each client socket for activity
        for (int i = 1; i <= MAX_FDS; i++) 
        {
            if (fds[i].revents & POLLIN) 
            {
                int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);

                if (bytesRead <= 0) {
                    // Client disconnected
                    sprintf(buffer, "server: client %d just left\n", i);

                    // Notify remaining clients about the disconnected client
                    for (int j = 1; j <= MAX_FDS; j++) 
                    {
                        if (fds[j].fd != -1 && fds[j].fd != fds[i].fd)
                        {
                            send(fds[j].fd, buffer, strlen(buffer), 0);
                        }
                    }

                    // Close the socket and remove it
                    close(fds[i].fd);
                    fds[i].fd = -1;
                } 
                else
                {
                    // check for the cmd from the clt
                    if (_clients.find(client_socket) != _clients.end())
                        checkCmd(_clients[client_socket], buffer);   
                    // Broadcast the received message to all other clients
                    buffer[bytesRead] = '\0';

                    char server_buffer[BUFFER_SIZE];
                    sprintf(server_buffer, "client %d: %s\n", i, buffer);

                    for (int j = 1; j <= MAX_FDS; j++) 
                    {
                        if (fds[j].fd != -1 && fds[j].fd != fds[i].fd) 
                        {
                            send(fds[j].fd, server_buffer, strlen(server_buffer), 0);
                        }
                    }
                }
            }
        }
    }
    // step 7: close the server/client sockets
    for (int i = 0; i < client_id; i++)
        close(client_sockets[i]);
    close(server_socket);
    return 0;
}
