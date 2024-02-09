#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include "server.hpp"
#include "main.hpp"

class Server
{
private:
    int _port;
    std::string _password;  
public:
    Server(int port, std::string password);
    ~Server();

    int getPort() const;
    std::string const getPassword() const;
    int ServerStartUp();
};

#endif