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

#include "commands.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "main.hpp"
#include "sig_utils.hpp"
#include "utils.hpp"

#define BUFFER_SIZE 10000
#define FDS_SIZE 1024

class Client;
class Channel;

class Server
{
private:
    int _port;
    std::string _password;
    std::map<int, Client*> _clients;
    std::map<std::string, Channel*> _channels;
    
    struct pollfd *pfds;

    int _server_socket;
    int _server_listener;
    int _senderFd;

public:
    Server(int port, std::string password);
    ~Server();

    //Server startup
    int ServerStartUp();
    //Server Cliente Related
    void AddClients(int &fd_count, int &MAX_FDS);
    //Server running
    void ServerIsRunning();
    int ServerListenerSock(void);
    void add_to_pfds(struct pollfd *pfds[], int client_fd, int *fd_count, int *fd_size);
    void del_from_pfds(struct pollfd pfds[], int i, int *fd_count);

    void ServerError(std::string error_str);
    void History();
    int Check_if_buf_cmd(char *buf);
    //void ServerListenerSock();
    //void AddPollFd(int listener_socket);

    //getters
    int getPort() const;
    std::string const getPassword() const;

    //commands
    void authProcess(Client *clt, char *cmd);
    void executeCmd(Client *clt, std::string cmd, std::string cmdValue);
    
};

#endif