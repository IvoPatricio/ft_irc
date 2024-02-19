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
#include <map>

#include "client.hpp"
#include "main.hpp"

class Client;

class Server
{
private:
    int _port;
    std::string _password;
    std::map<int, Client*> _clients;
public:
    Server(int port, std::string password);
    ~Server();

    int getPort() const;
    std::string const getPassword() const;
    void checkCmd(Client *clt, char *cmd);
    int ServerStartUp();
};

#endif