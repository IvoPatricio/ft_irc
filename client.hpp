#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "main.hpp"
#include "sig_utils.hpp"

class Client
{
private:
    int         _clientNum;
    bool        _admPerm;
    bool        _auth;
    bool        _userDef;
    bool        _nickDef;
    std::string _nick;
    std::string _username;
    std::string _password;
    std::string _channel;
    std::string _sendPrivate;
    std::string _receivePrivate;
    std::string _kick;
    std::string _invite;
    std::string _topic;
    std::string _mode;
    

public:
    Client(int clientNum);
    ~Client();

    // regular cmds
    void cmdPassword(std::string password, std::string serverPassword);
    void cmdNick(std::string nick);
    void cmdUsername(std::string username);
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
};

#endif