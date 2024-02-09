#include <string>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cctype>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "server.hpp"

Server::Server(int port, std::string password) : _port(port), _password(password)
{
    std::cout << "+Server Constructor called" << std::endl;
}

Server::~Server()
{
    std::cout << "-Server Destructor called" << std::endl;
}

int const Server::getPort() const
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
    return 0;
}
