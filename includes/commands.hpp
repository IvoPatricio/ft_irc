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

    static void joinChannel(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd);

};



#endif