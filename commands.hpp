#ifndef COMMANDS_HPP
# define COMMANDS_HPP

# include "server.hpp"

class Commands
{
private:
    Commands();
    ~Commands();

public:
    static void cmdNick(std::string nick);
    static void cmdUsername(std::string username);
    static void cmdChannel(std::string channel);
    static void cmdSendPrivate(std::string sendPrivate);
    static void cmdReceivePrivate(std::string receivePrivate);
    static void cmdKick(std::string kick);
    static void cmdInvite(std::string invite);
    static void cmdTopic(std::string topic);
    static void cmdMode(std::string mode);

    //setters needed? or inside the CMDs

};

#endif