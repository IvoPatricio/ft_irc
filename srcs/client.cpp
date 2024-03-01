#include "../includes/client.hpp"

Client::Client(int client_fd, struct sockaddr_in address) : _client_fd(client_fd), _address(address), _admPerm(false), _auth(false), _userDef(false), _nickDef(false), _nick(""), _username(""), _password("")
{
    std::cout << "Client created" << std::endl;
}

Client::~Client() {}


// regular cmds
void Client::cmdPassword(std::string insertPassword, std::string serverPassword)
{
    _password = insertPassword.substr(insertPassword.find_first_not_of(" ", 5));
    if (_password.compare(serverPassword) == 0)
        _auth = true;
    else
        error_print("Wrong Password");
    std::cout << "Password Accepted - You been authenticated" << _password << std::endl;
}

void Client::cmdUsername(std::string username)
{
    if (_auth)
    {
        _username = username.substr(username.find_first_not_of(" ", 9));
        _userDef = true;
        std::cout << "Your username is now " << _username << std::endl;
    }
    else
        error_print("Not Yet Authenticated - '/pass [password] to authenticate'");
}

void Client::cmdNick(std::string nick)
{
    if (_auth)
    {
        _nick = nick.substr(nick.find_first_not_of(" ", 5));
        _nickDef = true;
        std::cout << "Your nick is now " << _nick << std::endl;
    }
    else
        error_print("Not Yet Authenticated - '/pass [password] to authenticate'");
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
// {

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