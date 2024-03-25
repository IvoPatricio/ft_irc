#include "../includes/client.hpp"

Client::Client(int client_fd) : _client_fd(client_fd),
    _admPerm(false), _auth(false), _userDef(false), _nickDef(false), _nick(""), _username(""), _firstAuth(1)
{
    std::cout << "Client created" << std::endl;
    _firstAuth = 1;
}

Client::~Client() {}

// regular cmds
void Client::authenticate()
{
    _auth = true;
    std::cout << "Password Accepted - You been authenticated" << std::endl;
}

void Client::setUsername(std::string username)
{
    _username = username;
    _userDef = true;
    std::cout << "Your username is now " << _username << std::endl;
}

void Client::setNick(std::string nick)
{
    _nick = nick;
    _nickDef = true;
    std::cout << "Your nick is now " << _nick << std::endl;
}

void Client::setFirstAuth(int firstauth)
{
    _firstAuth = firstauth;
}

// void Client::cmdJoinChannel(std::string channel)
// {
//     (void)channel;
// }

// void Client::cmdSendChannel(std::string channel)
// {

// }

// void Client::cmdSendPrivate(std::string sendPrivate)
// {

// }

// void Client::cmdReceivePrivate(std::string receivePrivate)
// {

// }


// // operator cmds
// void Client::cmdKick(std::string kick)
// {]

// }

// void Client::cmdInvite(std::string invite)
// {

// }

// void Client::cmdTopic(std::string topic)
// {

// }

// void Client::cmdMode(std::string mode)
// {

// }

// getters
int Client::getFirstAuth()
{
    return _firstAuth;
}

bool Client::getAuth()
{
    return _auth;
}

bool Client::getUserDef()
{
    return _userDef;
}

bool Client::getNickDef()
{
    return _nickDef;
}

std::string Client::getNick()
{
    return _nick;
}

std::string Client::getUser()
{
    return _username;
}

int Client::getCltFd()
{
    return _client_fd;
}