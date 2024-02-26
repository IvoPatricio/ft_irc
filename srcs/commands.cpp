#include "../includes/commands.hpp"


Command::Command() {}

Command::~Command() {}

void Command::password(Client *clt, std::string insertPassword, std::string svPassword)
{
    if (insertPassword.compare(svPassword) == 0)
        clt->authenticate();
    else
        error_print("Wrong Password");
}

void Command::username(Client *clt, std::string username)
{
    // TODO: missing check if username too big and if already setted
    // username CANT change
    if (!checkOneWord(username))
    {
        error_print("Username has to be one word!");
        return ;
    }
    clt->setUsername(username);
}

void Command::nick(Client *clt, std::string nick)
{
    // TODO: missing check if username too big
    // nick CAN change
    if (!checkOneWord(nick))
    {
        error_print("Nick has to be one word!");
        return ;
    }
    clt->setNick(nick);
}

// /pmsg [user/nick] [msg]
void Command::pMsg(std::map<int, Client*> cltMap, int cltRecv, Client *cltSend, std::string cmd)
{
    (void)cltRecv;
    (void)cltSend;
    std::string msg[2];
    parseMsg(msg, cmd);
    std::map<int, Client*>::iterator it;
    for (it = cltMap.begin(); it != cltMap.end(); ++it)
    {
        if (it->second->getNick().compare(msg[0]) == 0)
        {
            //find clt
            //send msg
            return ;
        }
    }
    std::cout << "User nickname does not exist!" << std::endl; 
}

void Command::join(std::map<std::string, Channel*> &channelMap, Client *clt, std::string channelName)
{
    if (!checkOneWord(channelName))
    {
        error_print("Channel name has to be one word!");
        return ;
    }
    if (channelMap.find(channelName) != channelMap.end())
    {
        //channel exists
        channelMap[channelName]->addMember(clt);
        std::cout << "You joined " << channelName << std::endl;
    }
    else
    {
        channelMap[channelName] = new Channel(channelName, clt);
        std::cout << channelName << " channel created! " << std::endl;
    }
}


// void Command::kick(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd)
// {

// }

// void Command::invite(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd)
// {

// }

// void Command::topic(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd)
// {

// }

// void Command::mode(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd)
// {

// }
