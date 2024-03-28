#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "client.hpp"

class Client;

class Channel
{
private:
    std::string             _channelName;
    std::string             _channelPassword;
    std::string             _channelTopic;
    bool                    _inviteMode;
    bool                    _topicMode;
    bool                    _passwordMode;
    std::vector<Client*>    _memberList;
    std::vector<Client*>    _operatorList;

public:
    Channel(std::string channelName, Client *clt);
    ~Channel();

    void addMember(Client *clt);
    void addOperator(Client *clt);
    void removeMember(Client *clt);
    void removeOperator(Client *clt);

    // getters
    std::vector<Client*> &getMemberList();
    std::vector<Client*> &getOperatorList();
    std::string getChannelName();
    std::string getChannelTopic();

    //setters
    void setChannelTopic(std::string topic);

    //mode getters&setters
    bool getInviteMode();
    bool getTopicMode();
    bool getPasswordMode();
    void setInviteMode(bool value);
    void setTopicMode(bool value);
    void setPasswordMode(bool value);

};

#endif