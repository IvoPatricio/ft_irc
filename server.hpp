#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>

class Server
{
private:
    int _port;
    std::string _password;  
public:
    Server(int port, std::string password);
    ~Server();

    int const getPort() const;
    std::string const getPassword() const;
    int ServerStartUp();
};

#endif