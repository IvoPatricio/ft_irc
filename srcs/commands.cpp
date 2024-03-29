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
        sendIrcMessage(":@localhost 464 " + clt->getNick() + " :Password incorrect" , clt->getCltFd());
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
    sendIrcMessage(":@localhost 366 " + clt->getNick() + " " + channel->getChannelName() + " :End of NAMES list", clt->getCltFd());
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
    if (clt->getNick().empty())
    {
        sendIrcMessage(": NICK :" + nick, clt->getCltFd());
        clt->setNick(nick);
    }
    else
    {
        sendIrcMessage(":" + clt->getNick() + " NICK :" + nick, clt->getCltFd());
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
            for (size_t i = 0; i < channelMap[msg[0]]->getMemberList().size(); i++)
            {
                if (cltSend->getNick() == channelMap[msg[0]]->getMemberList()[i]->getNick())
                {
                    if (msg[1][0] != ':')
                    {
                        sendToAllInChannel(cltSend, channelMap[msg[0]]->getMemberList(), msg[1], msg[0]);
                    }
                    else
                    {
                        msg[1].erase(msg[1].begin());
                        sendToAllInChannel(cltSend, channelMap[msg[0]]->getMemberList(), msg[1], msg[0]);
                    }
                    return ;
                }
            }
            sendIrcMessage(":@localhost 404 " + cltSend->getNick() + " " + msg[0] + " :You are not in channel", cltSend->getCltFd());
        }
        else
        {
            sendIrcMessage(":@localhost 403 " + cltSend->getNick() + " " + msg[0] + " :Invalid channel name.", cltSend->getCltFd());
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

std::string getChannelPassword(std::string str)
{
    std::string channelPassword = "";
    size_t firstSpace = str.find_first_of(" ");
    size_t firstCharSecondWord = str.find_first_not_of(" ", firstSpace);
    if (firstCharSecondWord == std::string::npos)
        return "";
    size_t lastChar = str.find_last_not_of(" ");
    channelPassword = str.substr(firstCharSecondWord, lastChar);
    return channelPassword;
}

void joinExistinChannel(Channel *channel, Client *clt, std::string channelName)
{
    channel->addMember(clt);
    sendChannelMessage(":" + clt->getNick() + "!" + clt->getUser() + "@localhost" + " JOIN " + channelName, channel->getMemberList());
    sendIrcMessage(":@localhost 332 " + clt->getNick() + " " + channelName + " :" + channel->getChannelTopic(), clt->getCltFd());
    sendMembersToNewUser(channel, clt);
}

// usage -> /JOIN [channeName]
void Command::join(std::map<std::string, Channel*> &channelMap, Client *clt, std::string channelNameAndPassword)
{
    std::string channelName = channelNameAndPassword.substr(0, channelNameAndPassword.find_first_of(" ") - 1);
    std::string channelPassword = getChannelPassword(channelNameAndPassword);
    if(channelName[0] != '#')
    {
        sendIrcMessage(":@localhost 403 " + clt->getNick() + " " + channelName + " :Invalid channel name. Channel has to start with '#'", clt->getCltFd());
        return ;
    }
    if (channelMap.find(channelName) != channelMap.end())
    {
        if(channelMap[channelName]->getLimitMode() == true)
        {
            if (channelMap[channelName]->getMemberList().size() >= channelMap[channelName]->getLimitUsers())
            {
                sendIrcMessage(":@localhost 471 " + clt->getNick() + " " + channelName + " :Cannot join channel (+l)",  clt->getCltFd());
                return ;
            }
        }
        if (!channelMap[channelName]->getInviteMode())
        {
            if (channelMap[channelName]->getPasswordMode())
            {
                // has password
                if (channelPassword != channelMap[channelName]->getChannelPassword())
                {
                    sendIrcMessage(":@localhost 475 " + clt->getNick() + " " + channelName + " :Bad channel password", clt->getCltFd());
                    return ;
                }
            }    
            joinExistinChannel(channelMap[channelName], clt, channelName);
        }
        else
        {
            for (size_t i = 0; i < channelMap[channelName]->getInviteList().size(); i++)
            {
                if (clt->getNick() == channelMap[channelName]->getInviteList()[i]->getNick())
                {
                    joinExistinChannel(channelMap[channelName], clt, channelName);
                    return ;
                }
            }
            sendIrcMessage(":@localhost 473 " + clt->getNick() + " " + channelName + " :Invite only channel", clt->getCltFd());
        }
    }
    else
    {
        channelMap[channelName] = new Channel(channelName, clt);
        sendChannelMessage(":" + clt->getNick() + "!" + clt->getUser() + "@localhost" + " JOIN " + channelName, channelMap[channelName]->getMemberList());
        sendIrcMessage(":@localhost 332 " + clt->getNick() + " " + channelName + " :No topic is set", clt->getCltFd());
        channelMap[channelName]->setChannelTopic("No topic is set");
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
    }
    for (size_t i = 0; i < channelMap[channelNameCut]->getMemberList().size(); i++)
    {
        sendIrcMessage(":" + clt->getNick() + "@localhost PART " + channelName, channelMap[channelNameCut]->getMemberList()[i]->getCltFd());
        sendMembersToNewUser(channelMap[channelNameCut], channelMap[channelNameCut]->getMemberList()[i]);
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

void Command::quit(std::map<std::string, Channel*> &channelMap, std::map<int, Client*> &cltMap, Client *clt, int fd)
{
    sendIrcMessage(":" + clt->getNick() + "!@localhost QUIT :Quit: Bye for now!" , clt->getCltFd());
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

void Command::kick(std::map<std::string, Channel*> &channelMap, Client *clt, std::string user)
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
    size_t Pos1 = user.find(' ');
    size_t Pos2 = user.find(':');

    if (Pos1 == std::string::npos || Pos2 == std::string::npos)
    {
        sendIrcMessage(":@localhost 461 " + clt->getNick() + " " + user + " :Not enough parameters", clt->getCltFd());
		return;
    }
    std::string channelName = user.substr(Pos1 + 1, Pos2 - Pos1 - 2);
    std::string remaining = user.substr(Pos2 + 1);
    if (x != 2)
    {
        sendIrcMessage(":@localhost 461 " + clt->getNick() + " " + user + " :Not enough parameters", clt->getCltFd());
		return;
	}
    std::map<std::string, Channel*>::iterator it;
    for (it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        if (it->second->getChannelName() == channelName)
        {
            std::vector<Client*> operatorList = it->second->getOperatorList();
            for (size_t i = 0; i < operatorList.size(); ++i)
            {
                if (operatorList[i]->getNick() == clt->getNick())
                {
                    std::vector<Client*> memberList = it->second->getMemberList();
                    for (size_t i = 0; i < memberList.size(); ++i)
                    {
                        if (memberList[i]->getNick() == remaining)
                        {
                            sendIrcMessage(":" + clt->getNick() + " KICK " + channelName + " " + remaining + " (kicked)!:\r\n", memberList[i]->getCltFd());
                            channelMap[channelName]->removeMember(channelMap[channelName]->getMemberList()[i]);

                            std::vector<Client*> MemberPrint = channelMap[channelName]->getMemberList();
                            for (size_t i = 0; i < MemberPrint.size(); ++i) 
                            {
                                sendIrcMessage(":" + clt->getNick() + "!localhost KICK " + channelName + " " + remaining, channelMap[channelName]->getMemberList()[i]->getCltFd());
                                sendMembersToNewUser(channelMap[channelName], channelMap[channelName]->getMemberList()[i]);
                            }
                            return ;
                        }
                        if ((i + 1) == memberList.size())
                        {
                            sendIrcMessage(":@localhost 441 " + clt->getNick() + " " + remaining + " " + channelName + " :They aren't on that channel", clt->getCltFd());
		                    return;
                        }

                    }
                }
                if ((i + 1) == operatorList.size())
                {
                    sendIrcMessage(":@localhost 442 " + clt->getNick() + " " + channelName + " :You're not on that channel", clt->getCltFd());
		            return;
                }
            }
        }
    }
    sendIrcMessage(":@localhost 403 " + clt->getNick() + " " + channelName + " :No such channel", clt->getCltFd());
    return ;
}


void Command::topic(std::map<std::string, Channel*> &channelMap, Client *clt, std::string user, std::map<int, Client*> _clients)
{
    size_t Pos1 = user.find('#');
    size_t Pos2 = user.find(':');

    if (Pos1 == std::string::npos || Pos2 == std::string::npos) 
    {
        sendIrcMessage(":@localhost 461 " + clt->getNick() + " :Not enough parameters", clt->getCltFd());
        return ;
    }

    std::string channelName = user.substr(Pos1, Pos2 - Pos1 - 1);
    std::string remaining = user.substr(Pos2 + 1);
    std::map<std::string, Channel*>::iterator it;
    for (it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        if (it->second->getChannelName() == channelName)
        {
            if (it->second->getTopicMode() == true)
            {
                std::vector<Client*> operatorList = it->second->getOperatorList();
                for (size_t i = 0; i < operatorList.size(); ++i)
                {
                    if (operatorList[i]->getNick() == clt->getNick())
                    {
                        it->second->setChannelTopic(remaining);
                        std::map<int, Client*>::iterator it;
                        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
                        {
                            sendIrcMessage(":" + clt->getNick() + " TOPIC " + channelName + " " + remaining + "\r\n", it->second->getCltFd());
                        }
                        return ;
                    }
                    if ((i + 1) == operatorList.size())
                    {
                        sendIrcMessage(":@localhost 482 " + clt->getNick() + " " + channelName + " :You're not channel operator", clt->getCltFd());
                        return ;
                    }

                }
            }
            else if (it->second->getTopicMode() == false)
            {
                std::vector<Client*> memberList = it->second->getMemberList();
                for (size_t i = 0; i < memberList.size(); ++i)
                {
                    if (memberList[i]->getNick() == clt->getNick())
                    {
                        it->second->setChannelTopic(remaining);
                        std::map<int, Client*>::iterator it;
                        for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it)
                        {
                            sendIrcMessage(":" + clt->getNick() + " TOPIC " + channelName + " " + remaining + "\r\n", it->second->getCltFd());
                        }
                        return ;
                    }
                    if ((i + 1) == memberList.size())
                    {
                        sendIrcMessage(":@localhost 442 " + clt->getNick() + " " + channelName + " :You're not on that channel", clt->getCltFd());
                        return ;
                    }
                }
            }
        }
    }
    sendIrcMessage(":@localhost 403 " + clt->getNick() + " " + channelName + " :No such channel", clt->getCltFd());
    return ;
}

void Command::invite(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd, std::map<int, Client*> _clients)
{
    size_t Pos1 = cmd.find(' ');
    size_t Pos2 = cmd.find('#', Pos1);


    if (Pos1 == std::string::npos || Pos2 == std::string::npos)
    {
        sendIrcMessage(":@localhost 461 " + clt->getNick() + " " + cmd + " :Not enough parameters", clt->getCltFd());
		return;
    }
    std::string nick, channelName;
    nick = cmd.substr(0, Pos1);
    channelName = cmd.substr(Pos1 + 1);
    std::map<std::string, Channel*>::iterator it;
    for (it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        if (it->second->getChannelName() == channelName)
        {
            std::vector<Client*> memberList = it->second->getMemberList();
            for (size_t i = 0; i < memberList.size(); ++i)
            {
                if (memberList[i]->getNick() == clt->getNick())
                {
                    for (size_t i = 0; i < memberList.size(); ++i)
                    {
                        std::cout << "|" << nick << "|" << memberList[i]->getNick() << "|" << std::endl;
                        if (memberList[i]->getNick() == nick)
                        {
                            sendIrcMessage(":@localhost 443 " + clt->getNick() + " " + nick + " " + channelName + " :is already on channel", clt->getCltFd());
                            return ;
                        }
                        std::cout << i << memberList.size() << std::endl;
                        if ((i + 1) == memberList.size())
                        {
                            for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); ++it) 
                            {
                                std::string nickname = it->second->getNick();
                                if (it->second->getNick() == nick)
                                {
                                    channelMap[channelName]->addInviteList(it->second);
                                    // for (size_t i = 0; i < channelMap[channelName]->getInviteList().size(); i++)
                                    // {
                                    //     std::cout << "NAMES INV -> " << channelMap[channelName]->getInviteList()[i]->getNick() << std::endl;
                                    // }
                                    sendIrcMessage(clt->getNick() + " invited you to join " + channelName, it->second->getCltFd());
                                    sendIrcMessage(":@localhost 341 " + clt->getNick() + " " + nick + " " + channelName, clt->getCltFd());
                                    return ;
                                }
                            }
                        }
                    }
                }
                if ((i + 1) == memberList.size())
                {
                    sendIrcMessage(":@localhost 442 " + clt->getNick() + " " + channelName + " " + nick + " :client not found", clt->getCltFd());
                    return ;
                }
            }
        }
    }
    sendIrcMessage(":@localhost 403 " + clt->getNick() + " " + channelName + " :No such channel", clt->getCltFd());
    return ;
}

void Command::mode(std::map<std::string, Channel*> &channelMap, Client *clt, std::string cmd, std::map<int, Client*> _clients)
{
    size_t Pos1 = cmd.find('#');
    size_t Pos2 = cmd.find(' ', Pos1 + 1);
    size_t Pos3 = cmd.find(' ', Pos2 + 1);

    std::string channelName, remaining1, remaining2;
    if (Pos2 != std::string::npos)
    {
        channelName = cmd.substr(Pos1, Pos2 - Pos1);
        if (Pos3 != std::string::npos)
        {
            remaining1 = cmd.substr(Pos2 + 1, Pos3 - Pos2 - 1);
            remaining2 = cmd.substr(Pos3 + 1);
        }
        else
        {
            remaining1 = cmd.substr(Pos2 + 1);
            remaining2 = "";
        }
    }
    else
    {
        sendIrcMessage(":@localhost 482 " + clt->getNick() + " " + cmd + " :is unknown mode char to me", clt->getCltFd());
        return ;
    }
    std::map<std::string, Channel*>::iterator it;
    for (it = channelMap.begin(); it != channelMap.end(); ++it)
    {
        if (it->second->getChannelName() == channelName)
        {
            std::vector<Client*> operatorList = it->second->getOperatorList();
            for (size_t i = 0; i < operatorList.size(); ++i)
            {
                if (operatorList[i]->getNick() == clt->getNick())
                {
                    if (remaining1 == "+i")
                        it->second->setInviteMode(true);
                    else if (remaining1 == "-i")
                        it->second->setInviteMode(false);
                    else if (remaining1 == "+t")
                        it->second->setTopicMode(true);
                    else if (remaining1 == "-t")
                        it->second->setTopicMode(false);
                    else if (remaining1 == "+k")
                    {
                        it->second->setChannelPassword(remaining2);
                        it->second->setPasswordMode(true);
                    }
                    else if (remaining1 == "-k")
                        it->second->setPasswordMode(false);
                    else if (remaining1 == "+l")
                    {
                        const char* str = remaining2.c_str();
                        it->second->setLimitUsers(std::atoi(str));
                        it->second->setLimitMode(true);
                    }
                    else if (remaining1 == "-l")
                        it->second->setLimitMode(false);
                    else if (remaining1 == "+o" || remaining1 == "-o")
                    {
                        std::vector<Client*> operatorList = it->second->getOperatorList();
                        for (size_t i = 0; i < operatorList.size(); ++i)
                        {
                            if (operatorList[i]->getNick() == clt->getNick())
                            {
                                if (remaining1 == "+o")
                                {
                                    std::vector<Client*> memberList = it->second->getMemberList();
                                    for (size_t i = 0; i < memberList.size(); ++i)
                                    {
                                        if (memberList[i]->getNick() == remaining2)
                                        {
                                            it->second->addOperator(memberList[i]);
                                            std::vector<Client*> MemberPrint = channelMap[channelName]->getMemberList();
                                            for (size_t i = 0; i < MemberPrint.size(); ++i) 
                                            {
                                                sendMembersToNewUser(channelMap[channelName], channelMap[channelName]->getMemberList()[i]);
                                            }
                                            return ;
                                        }
                                    }
                                }
                                else if (remaining1 == "-o")
                                {
                                    std::vector<Client*> operatorList = it->second->getOperatorList();
                                    for (size_t i = 0; i < operatorList.size(); ++i)
                                    {
                                        if (operatorList[i]->getNick() == remaining2)
                                        {
                                            it->second->removeOperator(operatorList[i]);
                                            std::vector<Client*> MemberPrint = channelMap[channelName]->getMemberList();
                                            for (size_t i = 0; i < MemberPrint.size(); ++i) 
                                            {
                                                sendMembersToNewUser(channelMap[channelName], channelMap[channelName]->getMemberList()[i]);
                                            }
                                            return ;
                                        }
                                    }
                                }
                            }
                        }
                    }
                    return ;
                }
                if ((i + 1) == operatorList.size())
                {
                    sendIrcMessage(":@localhost 482 " + clt->getNick() + " " + channelName + " :You're not channel operator", clt->getCltFd());
                    return ;
                }
            }
        }
    }
    sendIrcMessage(":@localhost 485 " + clt->getNick() + " " + channelName + " :Cannot change mode due to channel restrictions", clt->getCltFd());
    return ;
}
