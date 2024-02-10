#include <string>
#include <cstring>
#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <stdlib.h>
#include <cctype>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include "server.hpp"
#include "main.hpp"

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

int Server::ServerStartUp()
{
    std::cout << "Server Port: " << getPort() << std::endl;
    std::cout << "Server Password: " << getPassword() << std::endl;

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
    while (1) 
    {
        // Step 4: Use poll to wait for events
        int poll_result = poll(fds, 1, -1);  // Blocking indefinitely
        if (poll_result == -1) 
        {
            error_print("Poll failed");
            break;
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
                        if (strncmp(buffer, "/nick", 5) == 0) 
                        {
                            std::cout << "/Nick" << std::endl;
                        }
                        int i = 0;
                        while (i < 99999)
                        {
                            std::cout << i << std::endl;
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
    return 0;
}
