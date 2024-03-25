#include "../includes/commands.hpp"


Command::Command() {}

Command::~Command() {}

int	sendIrcMessage(std::string message, int clientId)
{
	message = message + "\r\n";
	std::cout << "Sending message to clt id " << clientId << " : " << message << std::endl;
	if (send(clientId, message.c_str(), message.length(), 0) == -1)
		exit(error_print("Error sending message"));
    message.clear();
	return 0;
}

int	sendChannelMessage(std::string message, std::vector<Client*> clts)
{
    std::vector<Client*>::iterator it;
    for (it = clts.begin(); it < clts.end(); ++it)
    {
        sendIrcMessage(message, (*it)->getCltFd());
    }
	return 0;
}

void sendChannelToUser(Client *clt, std::string channelName, std::vector<Client*> clts)
{
    std::vector<Client*>::iterator it;
    for (it = clts.begin(); it < clts.end(); ++it)
    {
        // sendIrcMessage(":" + clt->getNick() + "!" + clt->getUser() + "@localhost" + " JOIN " + channelName, (*it)->getCltFd());
        sendIrcMessage(":" + (*it)->getNick() + "!" + (*it)->getUser() + "@localhost" + " JOIN " + channelName, clt->getCltFd());
        // sendIrcMessage(":" + (*it)->getNick() + " JOIN " + channelName, clt->getCltFd());
    }
}


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

// usage -> /NICK [nick]
// nick CAN change
void Command::nick(Client *clt, std::string nick)
{
    std::cout << "nick: " << clt->getCltFd();
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

void sendToAllInChannel(Client *cltSend, std::vector<Client*> clts, std::string msg, std::string channelName)
{
    std::vector<Client*>::iterator it;
    for (it = clts.begin(); it < clts.end(); ++it)
    {
        if ((*it)->getCltFd() != cltSend->getCltFd())
            sendIrcMessage(":" + cltSend->getNick() + "!@localhost PRIVMSG " + channelName + " :" + msg, (*it)->getCltFd());
    }
}

// /PRIVMSG [user/nick] [msg]
void Command::privMsg(std::map<std::string, Channel*> channelMap, std::map<int, Client*> cltMap, Client *cltSend, std::string cmd)
{
    (void)cltSend;
    std::string msg[2];
    parseMsg(msg, cmd);
    // std::cout << std::endl << "Pmsg:" << std::endl;
    // std::cout << "Nick -> " << msg[0] << std::endl;
    // std::cout << "Msg -> " << msg[1] << std::endl;
    if (msg[0][0] == '#')
    {
        if (channelMap.find(msg[0]) != channelMap.end())
        {
            //channel exists
            // sendIrcMessage("PRIVMSG " + msg[0], ->getCltFd());
    
            sendToAllInChannel(cltSend, channelMap[msg[0]]->getMemberList(), msg[1], msg[0]);
            // sendChannelMessage(":" + cltSend->getNick() + " PRIVMSG " + msg[0] + " :" + msg[1], it->second->getCltFd());
        }
        else
        {
            // does not exist
        }
    }
    std::map<int, Client*>::iterator it;
    for (it = cltMap.begin(); it != cltMap.end(); ++it)
    {
        if (it->second->getNick().compare(msg[0]) == 0)
        {
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
        }
    }
    std::cout << "User nickname does not exist!" << std::endl; 
}

// usage -> /JOIN [channeName]
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
        // std::cout << "You joined " << channelName << std::endl;
        // sendChannelMessage("JOIN #" + channelName, channelMap[channelName]->getMemberList());
        sendChannelMessage(":" + clt->getNick() + "!" + clt->getUser() + "@localhost" + " JOIN " + channelName, channelMap[channelName]->getMemberList());
        // sendIrcMessage("NAMES " + channelName, clt->getCltFd());
        // sendChannelToUser(clt, channelName, channelMap[channelName]->getMemberList());
    }
    else
    {
        channelMap[channelName] = new Channel(channelName, clt);
        // std::cout << "JOIN " << channelName << std::endl;
        sendChannelMessage(":" + clt->getNick() + "!" + clt->getUser() + "@localhost" + " JOIN " + channelName, channelMap[channelName]->getMemberList());
        // sendIrcMessage(":" + clt->getNick() + "!" + clt->getUser() + "@localhost" + " JOIN " + channelName, clt->getCltFd());
    }
}

void Command::quit(std::map<int, Client*> cltMap, std::vector<pollfd> pollfds, Client *clt, int fd)
{
    std::vector<pollfd>::iterator vecit;


    /*for (vecit = pollfds.begin(); vecit != pollfds.end(); ++vecit)
    {
        std::cout << "\nVEC:"<< vecit->fd << std::endl;
        if (vecit->fd == fd && vecit->fd != 3)
        {
            std::cout << "\nVEC:"<< vecit->fd << std::endl;
            close(fd);
            pollfds.erase(vecit);
        }
    }*/
    close(fd);
    std::map<int, Client*>::iterator it = cltMap.find(fd);
    if (it != cltMap.end()) 
    {
        delete it->second;
        cltMap.erase(it);
    }
}

void Command::kick(std::map<std::string, Channel*> channelMap, Client *clt, std::string user, std::map<int, Client*> _clients)
{
    std::cout << "Kicking" << std::endl;
    size_t Pos1 = user.find(' ');
    size_t Pos2 = user.find(':');

    std::string channelName = user.substr(Pos1 + 1, Pos2 - Pos1 - 2);
    std::string remaining = user.substr(Pos2 + 1);

    std::cout << "Channel Name: " << channelName << std::endl;
    std::cout << "Remaining: " << remaining << std::endl;

    std::map<std::string, Channel*>::iterator it;
    std::cout << "\n\n" << remaining << std::endl;
    for (it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        //Checking channel name
        if (it->second->getChannelName() == channelName)
        {
            std::vector<Client*> operatorList = it->second->getOperatorList();
            std::cout << "Operator List:" << std::endl;
            //checking the operators channel list
            for (size_t i = 0; i < operatorList.size(); ++i)
            {
                std::cout << "|OPERATOR|" << operatorList[i]->getNick() << clt->getNick() << "|OPERATOR|" << std::endl;
                if (operatorList[i]->getNick() == clt->getNick())
                {
                    std::cout << clt->getNick() << " is an operator" << std::endl;
                    std::vector<Client*> memberList = it->second->getMemberList();
                    //checking the members channel list
                    for (size_t i = 0; i < memberList.size(); ++i)
                    {
                        std::cout << "|MEMBER|" << memberList[i]->getNick() << remaining << "|MEMBER|" << std::endl;
                        if (memberList[i]->getNick() == remaining)
                            std::cout << remaining << " is a member channel" << std::endl;
                    }
                }
            }
        }
    }
    error_print("Invalid channel");
}

// void Command::invite(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd)
// {

// }

// void Command::topic(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd)
// {

// }

// void Command::mode(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd)
// {

// }
