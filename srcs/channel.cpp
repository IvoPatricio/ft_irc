#include "../includes/channel.hpp"

Channel::Channel(std::string channelName, Client *clt)
    : _channelName(channelName), _channelPassword(""), _channelTopic(""),
    _inviteMode(false), _topicMode(false), _passwordMode(false)
{
    _memberList.push_back(clt);
    _operatorList.push_back(clt);
}

Channel::~Channel()
{

}

void Channel::addMember(Client *clt)
{
    for (size_t i = 0; i < _memberList.size(); i++)
    {
        if (_memberList[i] == clt)
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

std::vector<Client*>& Channel::getMemberList()
{
    return _memberList;
}

std::vector<Client*>& Channel::getOperatorList()
{
    return _operatorList;
}