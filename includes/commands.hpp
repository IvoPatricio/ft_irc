#ifndef COMMANDS_HPP
# define COMMANDS_HPP

#include "main.hpp"
#include "client.hpp"
#include "channel.hpp"
#include "server.hpp"

class Channel;
class Client;
class Server;

class Command
{
private:
    Command();
    ~Command();
public:

    // regular cmds
    static void password(Client *clt, std::string insertPassword, std::string svPassword);
    static void username(Client *clt, std::string username);
    static bool nick(std::map<std::string, Channel*> ChannelMap, Client *clt, std::string nick);
    static void privMsg(std::map<std::string, Channel*> channelMap,std::map<int, Client*> cltMap, Client *cltSend, std::string cmd);
    static void join(std::map<std::string, Channel*> &channelMap, Client *clt, std::string channelName);
    static void quit(std::map<int, Client*> cltMap, std::vector<pollfd>	pollfds, Client *clt, int fd);
    
    // operator cmds
    // TODO: check args needed
    static void kick(std::map<std::string, Channel*> channelMap, Client *clt, std::string user, std::map<int, Client*> _clients);
    static void topic(std::map<std::string, Channel*> channelMap, Client *clt, std::string user, std::map<int, Client*> _clients);

    static void invite(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd);
    static void mode(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd);
};



#endif