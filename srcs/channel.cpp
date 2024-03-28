#include "../includes/channel.hpp"

Channel::Channel(std::string channelName, Client *clt)
    : _channelName(channelName), _channelPassword(""), _channelTopic(""),
    _inviteMode(false), _topicMode(false), _passwordMode(false)
{
    _memberList.push_back(clt);
    _operatorList.push_back(clt);
    _inviteMode = false;
    _topicMode = false;
    _passwordMode = false;
}

Channel::~Channel()
{

}

void Channel::addMember(Client *clt)
{
    for (size_t i = 0; i < _memberList.size(); i++)
    {
        if (_memberList[i]->getCltFd() == clt->getCltFd())
        {
            error_print("You are already in this channel!");
            return ;
        }
    }
    _memberList.push_back(clt);
    std::cout << "You joined the " << _channelName << " channel" << std::endl;
}

void Channel::addOperator(Client *clt)
{
    for (size_t i = 0; i < _operatorList.size(); i++)
    {
        if (_operatorList[i] == clt)
        {
            error_print("You are already a operator!");
            return ;
        }
    }
    _operatorList.push_back(clt);
    std::cout << "You are a operator now" << std::endl;
}

void Channel::removeMember(Client *clt)
{
    for (size_t i = 0; i < _memberList.size(); i++)
    {
        if (_memberList[i]->getCltFd() == clt->getCltFd())
        {
            _memberList.erase(_memberList.begin() + i);
            return ;
        }
    }
}

void Channel::removeOperator(Client *clt)
{
    for (size_t i = 0; i < _operatorList.size(); i++)
    {
        if (_operatorList[i] == clt)
        {
            _operatorList.erase(_operatorList.begin() + i);
            return ;
        }
    }
}

std::vector<Client*>& Channel::getMemberList()
{
    return _memberList;
}

std::vector<Client*>& Channel::getOperatorList()
{
    return _operatorList;
}

std::string Channel::getChannelName()
{
    return _channelName;
}

bool Channel::getInviteMode()
{
    return _inviteMode;
}

bool Channel::getTopicMode()
{
    return _topicMode;
}

bool Channel::getPasswordMode()
{
    return _passwordMode;
}

void Channel::setInviteMode(bool value)
{
    _inviteMode = value;
}

void Channel::setTopicMode(bool value)
{
    _topicMode = value;
}

void Channel::setPasswordMode(bool value)
{
    _passwordMode = value;
}