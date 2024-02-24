#include "../includes/commands.hpp"


Command::Command() {}

Command::~Command() {}

void Command::joinChannel(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd)
{
    if (!checkOneWord(cmd))
    {
        error_print("Channel name has to be one word!");
        return ;
    }
    if (channelMap.find(cmd) != channelMap.end())
    {
        channelMap[cmd]->addMember(clt);
        //channel exists
    }
    else
    {
        channelMap[cmd]->addMember(clt);
        channelMap[cmd]->addOperator(clt);
        channelMap[cmd] = new Channel(cmd, clt);
        std::cout << cmd << " channel created! " << std::endl;
    }
}