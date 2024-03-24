#include "../includes/commands.hpp"


Command::Command() {}

Command::~Command() {}

// usage -> /PASS [password]
void Command::password(Client *clt, std::string insertPassword, std::string svPassword)
{
    if (insertPassword.compare(svPassword) == 0)
        clt->authenticate();
    else
    {
        
        error_print("Wrong Password");
    }
}

// usage -> /USER [username]
// username CANT change
void Command::username(Client *clt, std::string username)
{
    if (clt->getUserDef())
    {
        error_print("Username already defiened");
        return ;
    }
    if (username.size() > 10)
    {
        error_print("Username too big! Max 10 characters");
        return ;
    }
    if (!checkOneWord(username))
    {
        error_print("Username can't be more than one word!");
        return ;
    }
    clt->setUsername(username);
}

int	sendIrcMessage(std::string message, int clientId)
{
	message = message + "\r\n";
	std::cout << "Sending message: " << message << std::endl;
	if (send(clientId, message.c_str(), message.length(), 0) == -1)
		exit(error_print("Error sending message"));
	return 0;
}

// usage -> /NICK [nick]
// nick CAN change
void Command::nick(Client *clt, std::string nick)
{
    if (nick.size() > 10)
    {
        error_print("Nick too big! Max 10 characters");
        return ;
    }
    if (!checkOneWord(nick))
    {
        error_print("Nick can't be more than one word!");
        return ;
    }
    sendIrcMessage(":" + clt->getNick() + " NICK :" + nick, clt->getCltFd());
    clt->setNick(nick);
}


// /PRIVMSG [user/nick] [msg]
void Command::privMsg(std::map<int, Client*> cltMap, Client *cltSend, std::string cmd)
{
    std::string msg[2];
    parseMsg(msg, cmd);
    // std::cout << std::endl << "Pmsg:" << std::endl;
    // std::cout << "Nick -> " << msg[0] << std::endl;
    // std::cout << "Msg -> " << msg[1] << std::endl;
    std::map<int, Client*>::iterator it;
    for (it = cltMap.begin(); it != cltMap.end(); ++it)
    {
        if (it->second->getNick().compare(msg[0]) == 0)
        {
            std::cout << "nick found!" <<std::endl;
            if (!msg[1].empty())
            {
                // sendIrcMessage(msg[1], it->second->getCltFd());
                if (msg[1][0] != ':')
                {
                    sendIrcMessage(":" + cltSend->getNick() + " PRIVMSG " + msg[0] + " :" + msg[1], it->second->getCltFd());
                    sendIrcMessage(":" + msg[0] + " PRIVMSG " + cltSend->getNick() + " :" + msg[1], cltSend->getCltFd());
                }
                else
                {
                    msg[1].erase(msg[1].begin());
                    sendIrcMessage(":" + cltSend->getNick() + " PRIVMSG " + msg[0] + " :" + msg[1], it->second->getCltFd());
                }
            }
            //find clt
            //send msg
            return ;
        }
    }
    std::cout << "User nickname does not exist!" << std::endl; 
}

int	sendChannelMessage(std::string message, std::vector<Client*> clts)
{
	// message = message + "\r\n";
    std::vector<Client*>::iterator it;
    for (it = clts.begin(); it < clts.end(); ++it)
    {
        sendIrcMessage(message, (*it)->getCltFd());
    }
	return 0;
}

// usage -> /JOIN [channeName]
void Command::join(std::map<std::string, Channel*> &channelMap, Client *clt, std::string channelName)
{
    
    // std::cout << "\ngetcltfd -> " << clt->getCltFd() << std::endl;
    if (!checkOneWord(channelName))
    {
        error_print("Channel name has to be one word!");
        return ;
    }
    if (channelMap.find(channelName) != channelMap.end())
    {
        //channel exists
        channelMap[channelName]->addMember(clt);
        // std::cout << "You joined " << channelName << std::endl;
        // sendChannelMessage("JOIN #" + channelName, channelMap[channelName]->getMemberList());
        sendIrcMessage(":" + clt->getNick() + "!" + clt->getUser() + "@localhost" + " JOIN " + channelName, clt->getCltFd());
    }
    else
    {
        channelMap[channelName] = new Channel(channelName, clt);
        // std::cout << "JOIN " << channelName << std::endl;
        sendIrcMessage(":" + clt->getNick() + "!" + clt->getUser() + "@localhost" + " JOIN " + channelName, clt->getCltFd());
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
