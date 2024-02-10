#ifndef SERVER_HPP
# define SERVER_HPP

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