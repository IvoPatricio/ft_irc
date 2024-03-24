#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "main.hpp"
#include "sig_utils.hpp"

class Client
{
private:
    int         _client_fd;
    struct sockaddr_in _address;
    int         _firstAuth;
    bool        _admPerm;
    bool        _auth;
    bool        _userDef;
    bool        _nickDef;
    std::string _nick;
    std::string _username;
    std::string _channel;
    std::string _sendPrivate;
    std::string _receivePrivate;
    std::string _kick;
    std::string _invite;
    std::string _topic;
    std::string _mode;
    

public:
    Client(int client_fd, struct sockaddr_in address);
    ~Client();

    // regular cmds
    void authenticate();
    void setUsername(std::string username);
    void setNick(std::string nick);
    void setFirstAuth(int firstAuth);
    void cmdJoinChannel(std::string channel);
    void cmdSendChannel(std::string channel);
    void cmdSendPrivate(std::string sendPrivate);
    void cmdReceivePrivate(std::string receivePrivate);

    // operator cmds
    void cmdKick(std::string kick);
    void cmdInvite(std::string invite);
    void cmdTopic(std::string topic);
    void cmdMode(std::string mode);

    //getters
    bool getAuth();
    bool getUserDef();
    bool getNickDef();
    std::string getUser();
    std::string getNick();
    int getCltFd();
    int getFirstAuth();
};

#endif