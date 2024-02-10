#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>  // Include this for fcntl
#include <algorithm> // Include this for std::remove_if
#include <poll.h>

#include "main.hpp"

class Server
{
private:
    int _port;
    std::string _password;  
public:
    Server(int port, std::string password);
    ~Server();

    int getPort() const;
    std::string const getPassword() const;
    int ServerStartUp();
};

#endif