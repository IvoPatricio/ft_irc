#include "../includes/channel.hpp"

Channel::Channel(std::string channelName, Client *clt)
    : _channelName(channelName), _channelPassword(""), _limitMode(false), _channelTopic(""), _password(""), _userLimit(0),
    _inviteMode(false), _topicMode(false), _passwordMode(false)
{
    _memberList.push_back(clt);
    _operatorList.push_back(clt);
    _inviteMode = false;
    _limitMode = false;
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
    //std::cout << "You joined the " << _channelName << " channel" << std::endl;
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
    //std::cout << "You are a operator now" << std::endl;
}

void Channel::addInviteList(Client *clt)
{
    for (size_t i = 0; i < _inviteList.size(); i++)
    {
        if (_inviteList[i] == clt)
        {
            error_print("You are already invited");
            return ;
        }
    }
    _inviteList.push_back(clt);
    //std::cout << "You are invited now" << std::endl;
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

bool Channel::getLimitMode()
{
    return _limitMode;
}

size_t Channel::getLimitUsers()
{
    return static_cast<size_t>(_userLimit);
}

std::string Channel::getChannelTopic()
{
    return _channelTopic;
}

void Channel::setChannelTopic(std::string topic)
{
    _channelTopic = topic;
}

std::vector<Client*>& Channel::getMemberList()
{
    return _memberList;
}

std::vector<Client*>& Channel::getOperatorList()
{
    return _operatorList;
}

std::vector<Client*>& Channel::getInviteList()
{
    return _inviteList;
}

std::string Channel::getChannelName()
{
    return _channelName;
}

std::string Channel::getChannelPassword()
{
    return _password;
}

void Channel::setChannelPassword(std::string password)
{
    _password = password;
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

void Channel::setLimitMode(bool value)
{
    _limitMode = value;
}

void Channel::setLimitUsers(int limit)
{
    _userLimit = limit;
}