#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#include "main.hpp"
#include "client.hpp"
#include "channel.hpp"

class Channel;
class Client;

class Command
{
private:
    Command();
    ~Command();
public:

    // regular cmds
    static void password(Client *clt, std::string insertPassword, std::string svPassword);
    static void username(Client *clt, std::string username);
    static void nick(Client *clt, std::string nick);
    static void pMsg(std::map<int, Client*> cltMap, Client *cltSend, std::string cmd);
    static void join(std::map<std::string, Channel*> &channelMap, Client *clt, std::string channelName);
    
    // operator cmds
    // TODO: check args needed
    static void kick(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd);
    static void invite(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd);
    static void topic(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd);
    static void mode(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd);
};



#endif