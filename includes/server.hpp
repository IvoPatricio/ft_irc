#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm> 
#include <poll.h>
#include <csignal>
#include <map>

#include "commands.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "main.hpp"
#include "sig_utils.hpp"

#define MAX_FDS 1024
#define BUFFER_SIZE 200000

class Client;
class Channel;

class Server
{
private:
    int _port;
    std::string _password;
    std::map<int, Client*> _clients;
    std::map<std::string, Channel*> _channels;

public:
    Server(int port, std::string password);
    ~Server();

    //Server startup
    int ServerStartUp();

    //getters
    int getPort() const;
    std::string const getPassword() const;

    //commands
    void authProcess(Client *clt, char *cmd);
    void executeCmd(Client *clt, char *cmd);
    
};

#endif