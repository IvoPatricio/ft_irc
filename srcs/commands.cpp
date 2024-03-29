#include "../includes/commands.hpp"


Command::Command() {}

Command::~Command() {}

int	sendChannelMessage(std::string message, std::vector<Client*> clts)
{
    std::vector<Client*>::iterator it;
    for (it = clts.begin(); it < clts.end(); ++it)
    {
        sendIrcMessage(message, (*it)->getCltFd());
    }
	return 0;
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

void sendMembersToNewUser(Channel *channel, Client *clt)
{
    std::string nickListString;
    bool op = false;
    std::vector<Client*>::iterator it;
    for (it = channel->getMemberList().begin(); it < channel->getMemberList().end(); ++it)
    {
        op = false;
        for (size_t i = 0; i < channel->getOperatorList().size(); i++)
        {
            if ((*it)->getNick() == channel->getOperatorList()[i]->getNick())
            {
                nickListString += "@" + (*it)->getNick() + " ";
                op = true;
            }
        }
        if (op == false)
            nickListString += (*it)->getNick() + " ";
    }
    sendIrcMessage(":@localhost 353 " + clt->getNick() + " = " + channel->getChannelName() + " :" + nickListString, clt->getCltFd());
}

void updateNickInChannels(std::map<std::string, Channel*> channelMap, Client *clt, std::string oldNick)
{
    std::map<std::string, Channel*>::iterator it;
    for (it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        size_t size = it->second->getMemberList().size();
        for (size_t i = 0; i < size ; i++)
        {
            if (it->second->getMemberList()[i]->getNick() == clt->getNick())
            {
                for (size_t j = 0; j < size; j++)
                {
                    sendIrcMessage(":" + oldNick + " NICK :" + clt->getNick(), it->second->getMemberList()[j]->getCltFd());
                }
                break ;
            }
        }
    }
}

// usage -> /NICK [nick]
// nick CAN change
bool Command::nick(std::map<std::string, Channel*> channelMap, Client *clt, std::string nick)
{
    std::string oldNick = "";
    std::cout << "nick: " << clt->getCltFd();
    if (!checkOneWord(nick))
    {
        error_print("Nick can't be more than one word!");
        return false;
    }
    sendIrcMessage(":" + clt->getNick() + " NICK :" + nick, clt->getCltFd());
    if (clt->getNick().empty())
        clt->setNick(nick);
    else
    {
        oldNick = clt->getNick();
        clt->setNick(nick);
        updateNickInChannels(channelMap, clt, oldNick);
    }
    return true;
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
    // (void)cltSend;
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
            msg[1].erase(msg[1].begin());
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
}

// usage -> /JOIN [channeName]
void Command::join(std::map<std::string, Channel*> &channelMap, Client *clt, std::string channelName)
{
    if(channelName[0] != '#')
    {
        sendIrcMessage(":@localhost 403 " + clt->getNick() + " " + channelName + " :Invalid channel name. Channel has to start with '#'", clt->getCltFd());
        return ;
    }
    if (!checkOneWord(channelName))
    {
        sendIrcMessage(":@localhost 403 " + clt->getNick() + " " + channelName + " :Bad channel name", clt->getCltFd());
        error_print("Channel name has to be one word!");
        return ;
    }
    if (channelMap.find(channelName) != channelMap.end())
    {
        //channel exists
        channelMap[channelName]->addMember(clt);
        sendChannelMessage(":" + clt->getNick() + "!" + clt->getUser() + "@localhost" + " JOIN " + channelName, channelMap[channelName]->getMemberList());
        sendMembersToNewUser(channelMap[channelName], clt);
    }
    else
    {
        channelMap[channelName] = new Channel(channelName, clt);
        sendChannelMessage(":" + clt->getNick() + "!" + clt->getUser() + "@localhost" + " JOIN " + channelName, channelMap[channelName]->getMemberList());
        sendMembersToNewUser(channelMap[channelName], clt);
        // sendIrcMessage(":" + clt->getNick() + "!" + clt->getUser() + "@localhost" + " JOIN " + channelName, clt->getCltFd());
        // sendIrcMessage(":@localhost 366" + clt->getNick() + " " + channelName + ":End of NAMES list", clt->getCltFd());
    }
}

void Command::part(std::map<std::string, Channel*> &channelMap, Client *clt, std::string channelName)
{
    std::string channelNameCut = channelName.substr(0, channelName.find_first_of(" "));
    for (size_t i = 0; i < channelMap[channelNameCut]->getMemberList().size(); i++)
    {
        if (clt->getNick() == channelMap[channelNameCut]->getMemberList()[i]->getNick())
        {
            channelMap[channelNameCut]->removeMember(clt);
            for (size_t i = 0; i < channelMap[channelNameCut]->getOperatorList().size(); i++)
            {
                if (clt->getNick() == channelMap[channelNameCut]->getOperatorList()[i]->getNick())
                    channelMap[channelNameCut]->removeOperator(clt);
            }
        }
        else
            sendIrcMessage(":" + clt->getNick() + " PART " + channelNameCut + ":User left the channel", channelMap[channelNameCut]->getMemberList()[i]->getCltFd());
    }
}

void removeUserFromAllChannels(std::map<std::string, Channel*> &channelMap, Client *clt)
{
    std::map<std::string, Channel*>::iterator it;
    for (it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        for (size_t i = 0; i < it->second->getMemberList().size(); i++)
        {
            if (it->second->getMemberList()[i]->getCltFd() == clt->getCltFd())
            {
                Command::part(channelMap, clt, it->first);
                break;
            }
        }
    }
} 

void Command::quit(std::map<std::string, Channel*> &channelMap, std::map<int, Client*> &cltMap, std::vector<pollfd> pollfds, Client *clt, int fd)
{
    /*std::vector<pollfd>::iterator vecit;
    for (vecit = pollfds.begin(); vecit != pollfds.end(); ++vecit)
    {
        std::cout << "\nVEC:"<< vecit->fd << std::endl;
        if (vecit->fd == fd && vecit->fd != 3)
        {
            std::cout << "\nVEC:"<< vecit->fd << std::endl;
            close(fd);
            pollfds.erase(vecit);
        }
    }*/
    removeUserFromAllChannels(channelMap, clt);
    std::map<int, Client*>::iterator it = cltMap.find(fd);
    if (it != cltMap.end()) 
    {
        delete it->second;
        it->second = NULL;
        cltMap.erase(it);
    }
    close(fd);
}

void Command::kick(std::map<std::string, Channel*> channelMap, Client *clt, std::string user, std::map<int, Client*> _clients)
{
    unsigned int x = 0;
	unsigned int i = 0;

	while (user.size() > i)
	{
		while (i < user.size() && (user[i] != ' ' && user[i] != '\r' && user[i] != '\n'))
			i++;
        if (i < user.size() && (user[i] == ' ' || user[i] == '\r' || user[i] == '\n'))
			x++;
		while (i < user.size() && (user[i] == ' ' || user[i] == '\r' || user[i] == '\n'))
			i++;
	}
    std::cout << "\n\n" << x << "\n\n" << std::endl;
	if (x != 2)
    {
		error_print("Wrong KICK format. /kick #channel user_name");
		return;
	}
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
                        {
                            std::cout << remaining << " is a member channel" << std::endl;
                            sendIrcMessage(":" + clt->getNick() + " KICK " + channelName + " " + remaining + " (kicked)!:\r\n", memberList[i]->getCltFd());
                            memberList.erase(memberList.begin() + i);
                            return ;
                        }
                        sendIrcMessage(":" + clt->getNick() + " KICK " + channelName + " " + remaining + " (kicked)!:\r\n", memberList[i]->getCltFd());
                    }
                }
            }
        }
    }
    error_print("Invalid KICK");
}


void Command::topic(std::map<std::string, Channel*> channelMap, Client *clt, std::string user, std::map<int, Client*> _clients)
{
    std::cout << "topic" << std::endl;
    size_t Pos1 = user.find('#');
    size_t Pos2 = user.find(':');

    std::string channelName = user.substr(Pos1, Pos2 - Pos1 - 1);
    std::string remaining = user.substr(Pos2 + 1);
    std::cout << channelName << "|" << remaining << std::endl;
    std::map<std::string, Channel*>::iterator it;
    for (it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        if (it->second->getChannelName() == channelName)
        {
            ;
        }
    }
    error_print("Invalid TOPIC");
}

// void Command::invite(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd)
// {

// void Command::mode(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd)
// {

// }
