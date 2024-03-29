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
    std::string             _password;
    bool                    _inviteMode;
    bool                    _topicMode;
    bool                    _passwordMode;
    bool                    _limitMode;
    std::vector<Client*>    _memberList;
    std::vector<Client*>    _operatorList;
    std::vector<Client*>    _inviteList;
    int                     _userLimit;

public:
    Channel(std::string channelName, Client *clt);
    ~Channel();

    void addMember(Client *clt);
    void addOperator(Client *clt);
    void addInviteList(Client *clt);
    void removeMember(Client *clt);
    void removeOperator(Client *clt);

    // getters
    std::vector<Client*> &getMemberList();
    std::vector<Client*> &getOperatorList();
    std::vector<Client*> &getInviteList();
    std::string getChannelName();
    std::string getChannelTopic();
    std::string getChannelPassword();

    //setters
    void setChannelTopic(std::string topic);
    void setChannelPassword(std::string password);
    void setLimitUsers(int limit);
    void setLimitMode(bool value);
    void setInviteMode(bool value);
    void setTopicMode(bool value);
    void setPasswordMode(bool value);

    //mode getters&setters
    bool getInviteMode();
    bool getTopicMode();
    bool getPasswordMode();
    bool getLimitMode();
    int getLimitUsers();

};

#endif