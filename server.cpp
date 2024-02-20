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

int Server::ServerStartUp()
{
    std::cout << "Server Port: " << getPort() << std::endl;
    std::cout << "Server Password: " << getPassword() << std::endl;

    signal_handlers();
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    // Step 1: Create a socket
    if (server_socket == -1) 
    {
        error_print("Socket creation failed");
        return 1;
    }

    // Step 2: Bind the socket to a specific address and port
    sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(getPort());

    if (bind(server_socket, reinterpret_cast<struct sockaddr*>(&server_address), sizeof(server_address)) == -1) 
    {
        error_print("Bind Failed");
        close(server_socket);
        return 1;
    }

    // Step 3: Listen for incoming connections
    if (listen(server_socket, 1) == -1) 
    {
        error_print("Listen failed");
        close(server_socket);
        return 1;
    }
    std::cout << "Server listening on port " << getPort() << std::endl;

    // Set up pollfd structure
    struct pollfd fds[1];
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;

    // Main server loop
    while (isRunning == true) 
    {
        // Step 4: Use poll to wait for events
        int poll_result = poll(fds, 1, -1);  // Blocking indefinitely
        if (poll_result == -1) 
        {
            error_print("Poll failed");
            break;
        }
        //https://www.youtube.com/watch?v=dEHZb9JsmOU
        if (poll_result > 0) 
        {
            if (fds[0].revents & POLLIN) 
            {
                // Step 5: Accept a connection
                int client_socket = accept(server_socket, NULL, NULL);
                if (client_socket == -1)
                {
                    error_print("Accept failed");
                    break;
                }
                // adding a clt to the map
                _clients[client_socket] = new Client(client_socket);
                // Step 6: Read and write data
                char buffer[1024];
                ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
                if (bytes_received == -1)
                {
                    error_print("Recv failed");
                    break;
                }
                if (bytes_received > 0) 
                {
                    buffer[bytes_received] = '\0';
                    std::cout << "Client connected to the server: " << client_socket << std::endl;
                    while (1)
                    {
                        const char *response_data = "Client connected to the server!";
                        if (send(client_socket, response_data, strlen(response_data), 0) == -1)
                        {
                            error_print("Send failed");
                            break;
                        }
                        // if (send(client_socket, response_data, strlen(response_data), 0) == -1)
                        // {
                        //     error_print("Send failed");
                        //     break;
                        // }
                        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
                        if (bytes_received == -1)
                        {
                            error_print("Recv failed");
                            break;
                        }
                        if (bytes_received == 0)
                        {
                            std::cout << "Client disconnected" << std::endl;
                            close(client_socket);
                            break;
                        }
    
                        buffer[bytes_received] = '\0';
                        std::cout << "Received from client " << client_socket << ": " << buffer << std::endl;
                        // check for the cmd from the clt
                        if (_clients.find(client_socket) != _clients.end())
                            checkCmd(_clients[client_socket], buffer);
                    }

                    // Simulate sending a response back to the client
                    const char *response_data = "Client connected to the server!";
                    if (send(client_socket, response_data, strlen(response_data), 0) == -1)
                    {
                        error_print("Send failed");
                        break;
                    }
                }
                std::cout << "Client disconnected" << std::endl;
                close(client_socket);
            }
        }
    }
    // Step 8: Close the server socket
    close(server_socket);
    return 0;
}
