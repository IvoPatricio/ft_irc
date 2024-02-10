#include "server.hpp"

Server::Server(int port, std::string password) : _port(port), _password(password)
{
    std::cout << "+Server Constructor called" << std::endl;
}

Server::~Server()
{
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


const int MAX_CLIENTS = 10;
const int MAX_BUFFER_SIZE = 1024;

struct Client {
    int socket;
    std::string nickname;
    // Add more fields as needed
};

std::vector<Client> clients;

void handleNewConnection(int serverSocket, std::vector<pollfd>& pollFds) {
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
    // Handle error checking for accept()

    // Set non-blocking
    fcntl(clientSocket, F_SETFL, fcntl(clientSocket, F_GETFL, 0) | O_NONBLOCK);

    // Add the new client to the vector
    Client newClient;
    newClient.socket = clientSocket;
    newClient.nickname = "";
    clients.push_back(newClient);

    std::cout << "New connection from " << inet_ntoa(clientAddr.sin_addr) << std::endl;

    // Add new client to pollFds
    pollfd newClientPollfd;
    newClientPollfd.fd = clientSocket;
    newClientPollfd.events = POLLIN;
    newClientPollfd.revents = 0;
    pollFds.push_back(newClientPollfd);
}

void handleDataFromClient(size_t clientIndex, std::vector<pollfd>& pollFds) {
    char buffer[MAX_BUFFER_SIZE];
    int bytesRead = recv(clients[clientIndex].socket, buffer, sizeof(buffer), 0);

    if (bytesRead > 0) {
        // Process received data
        // Implement IRC protocol handling here
    } else if (bytesRead == 0 || (bytesRead == -1 && errno != EWOULDBLOCK)) {
        // Connection closed or error (other than non-blocking)
        // Remove the client from the vector
        close(clients[clientIndex].socket);
        clients.erase(clients.begin() + clientIndex);
        pollFds.erase(pollFds.begin() + clientIndex + 1); // +1 to account for server socket at index 0
    }
}

int Server::ServerStartUp()
{
    std::cout << "Server Port: " << getPort() << std::endl;
    std::cout << "Server Password: " << getPassword() << std::endl;

    /*int server_socket = socket(AF_INET, SOCK_STREAM, 0);
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
    while (1) 
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
                        const char *response_const char *response_data = "Client connected to the server!";
                    if (send(client_socket, response_data, strlen(response_data), 0) == -1)
                    {
                        error_print("Send failed");
                        break;
                    }data = "Client connected to the server!";
                        if (send(client_socket, response_data, strlen(response_data), 0) == -1)
                        {
                            error_print("Send failed");
                            break;
                        }
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
    
                        if (strncmp(buffer, "/nick", 5) == 0) 
                        {
                            std::cout << "/nick" << std::endl;
                        }
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
    std::cout << "test" << std::endl;
    // Step 8: Close the server socket
    close(server_socket);
    return 0;*/
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    // Handle error checking for socket()

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(getPort()); // Change port as needed

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    // Handle error checking for bind()

    listen(serverSocket, MAX_CLIENTS);
    // Handle error checking for listen()

    std::vector<pollfd> pollFds;

    pollfd serverPollfd;
    serverPollfd.fd = serverSocket;
    serverPollfd.events = POLLIN;
    serverPollfd.revents = 0;
    pollFds.push_back(serverPollfd);

    while (true) {
        int events = poll(&pollFds[0], pollFds.size(), -1);

        if (events > 0) {
            for (size_t i = 0; i < pollFds.size(); ++i) {
                if (pollFds[i].revents & POLLIN) {
                    if (pollFds[i].fd == serverSocket) {
                        handleNewConnection(serverSocket, pollFds);
                    } else {
                        handleDataFromClient(i - 1, pollFds);
                    }
                }
            }
        }
    }
    close(serverSocket);
    return 0;
}
