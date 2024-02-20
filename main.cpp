#include "main.hpp"

int error_print(std::string error_str)
{
    std::cerr << RED << "ERROR - " << YELLOW << error_str << RESET << std::endl;
    return 1;
}

int string_chart(std::string pass)
{
    unsigned long i = 0;

    if (pass.size() > 10 || pass.size() < 3)
        return 1;
    while (i < pass.size())
    {
        if (isalnum(pass.at(i)) == 0)
            return 2;
        i++;
    }
    return 0;
}

int string_digit(std::string port)
{
    unsigned long i = 0;

    if (port.size() > 5 || atoi(port.c_str()) <= 1023)
        return 1;
    while (i < port.size())
    {
        if (isdigit(port.at(i)) == 0)
            return 2;
        i++;
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3)
        return (error_print("Arguments have to be 3 (./ircserv <port> <password>)"));

    if (string_digit(argv[1]) != 0)
    {
        if (string_digit(argv[1]) == 1)
            return (error_print("Invalid port value"));
        else
            return (error_print("Invalid port size"));
    }

    if (string_chart(argv[2]) != 0)
    {
        if (string_chart(argv[2]) == 1)
            return (error_print("Invalid password size"));
        else
            return (error_print("Invalid password value"));
    }

    Server ServerObj = Server(atoi(argv[1]), argv[2]);
    ServerObj.ServerStartUp();

    return 0;
    
}

/*
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <poll.h>

#define MAX_CLIENTS 1024
#define BUFFER_SIZE 200000

int main(int argc, char **argv) 
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    int clientSockets[MAX_CLIENTS];
    int next_id = 0;

    struct pollfd fds[MAX_CLIENTS + 1];  // +1 for the server socket
    char buffer[BUFFER_SIZE];

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) 
    {
        perror("Error creating server socket");
        exit(1);
    }

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(atoi(argv[1]));

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Error binding server socket");
        exit(1);
    }

    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        perror("Error listening on server socket");
        exit(1);
    }

    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;

    for (int i = 1; i <= MAX_CLIENTS; i++) {
        fds[i].fd = -1;  // Initialize client sockets to -1
    }

    int maxSocket = serverSocket;

    while (1) 
    {
        int poll_result = poll(fds, maxSocket + 1, -1);
        if (poll_result < 0) {
            perror("Error in poll");
            exit(1);
        }

        // Check server socket for new connection
        if (fds[0].revents & POLLIN) 
        {
            int clientSocket = accept(serverSocket, NULL, NULL);
            if (clientSocket < 0) 
            {
                perror("Error accepting client connection");
                exit(1);
            }

            // Add the new client socket to the array
            for (int i = 1; i <= MAX_CLIENTS; i++) 
            {
                if (fds[i].fd == -1) 
                {
                    fds[i].fd = clientSocket;
                    fds[i].events = POLLIN;
                    break;
                }
            }

            if (clientSocket > maxSocket)
                maxSocket = clientSocket;

            // Send a welcome message to the client
            sprintf(buffer, "server: client %d just arrived\n", next_id);
            send(clientSocket, buffer, strlen(buffer), 0);

            // Store the client socket for future reference
            clientSockets[next_id++] = clientSocket;
        }

        // Check each client socket for activity
        for (int i = 1; i <= MAX_CLIENTS; i++) 
        {
            if (fds[i].revents & POLLIN) 
            {
                int bytesRead = recv(fds[i].fd, buffer, sizeof(buffer) - 1, 0);

                if (bytesRead <= 0) {
                    // Client disconnected
                    sprintf(buffer, "server: client %d just left\n", i);

                    // Notify remaining clients about the disconnected client
                    for (int j = 1; j <= MAX_CLIENTS; j++) {
                        if (fds[j].fd != -1 && fds[j].fd != fds[i].fd) {
                            send(fds[j].fd, buffer, strlen(buffer), 0);
                        }
                    }

                    // Close the socket and remove it from the array
                    close(fds[i].fd);
                    fds[i].fd = -1;
                } 
                else
                {
                    // Broadcast the received message to all other clients
                    buffer[bytesRead] = '\0';

                    char broadcastBuffer[BUFFER_SIZE];
                    sprintf(broadcastBuffer, "client %d: %s\n", i, buffer);

                    for (int j = 1; j <= MAX_CLIENTS; j++) {
                        if (fds[j].fd != -1 && fds[j].fd != fds[i].fd) {
                            send(fds[j].fd, broadcastBuffer, strlen(broadcastBuffer), 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}*/

