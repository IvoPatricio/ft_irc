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
#include <fcntl.h>
#include <algorithm> 
#include <poll.h>
#include <csignal>

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