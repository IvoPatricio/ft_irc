#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "main.hpp"

class Client
{
private:
    int         _client_fd;
    struct sockaddr_in _address;
    int         _firstAuth;
    bool        _invitePerm;
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
    Client(int client_fd);
    ~Client();

    // regular cmds
    void authenticate();
    void cmdJoinChannel(std::string channel);
    void cmdSendChannel(std::string channel);
    void cmdSendPrivate(std::string sendPrivate);
    void cmdReceivePrivate(std::string receivePrivate);

    // operator cmds
    void cmdKick(std::string kick);
    void cmdInvite(std::string invite);
    void cmdTopic(std::string topic);
    void cmdMode(std::string mode);

    //setters
    void setInvitePerm(bool value);
    void setUsername(std::string username);
    void setNick(std::string nick);
    void setFirstAuth(int firstAuth);

    //getters
    bool getAuth();
    bool getUserDef();
    bool getNickDef();
    std::string getUser();
    std::string getNick();
    int getCltFd();
    int getFirstAuth();
    bool getInvitePerm();

};

#endif