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
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h> 
#include <netinet/tcp.h>
#include <sstream>

#include "commands.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "main.hpp"
#include "sig_utils.hpp"
#include "utils.hpp"

#define BUFFER_SIZE 10000
#define MAX_USER 1024

class Client;
class Channel;

class Server
{
private:
    int _port;
    std::string _password;

    std::map<int, Client*> _clients;
    std::map<std::string, Channel*> _channels;
    std::vector<pollfd>	pollfds;
    struct sockaddr_in server_address;

    int _server_socket;
    int _client_socket;

public:
    Server(int port, std::string password);
    ~Server();

    //Server startup
    int ServerStartUp();
    //Cliente
    void AddClients();
    //Server running
    void ServerListenerSock();
    void ServerError(std::string error_str);
    void parseInitialMsg(Client *clt, int fd, char* fullCmd);
    int nickChecker(Client *clt, std::string nick);
    int userChecker(Client *clt, std::string user);

    //getters
    int getPort() const;
    std::string const getPassword() const;

    //commands
    void authProcess(Client *clt, int fd, char *fullcmd);
    void executeCmd(Client *clt, std::string cmd, std::string cmdValue);
    void authFirstSettings(Client *clt, char *fullCmd);
    
};

#endif