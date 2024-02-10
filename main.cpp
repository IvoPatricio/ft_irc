#include "main.hpp"

int error_print(std::string error_str)
{
    std::cerr << RED << "ERROR - " << YELLOW << error_str << RESET << std::endl;
    return 1;
}

int string_chart(std::string pass)
{
    unsigned long i = 0;

    if (pass.size() > 10 || pass.size() < 3)
        return 1;
    while (i < pass.size())
    {
        if (isalnum(pass.at(i)) == 0)
            return 2;
        i++;
    }
    return 0;
}

int string_digit(std::string port)
{
    unsigned long i = 0;

    if (port.size() > 5)
        return 1;
    while (i < port.size())
    {
        if (isdigit(port.at(i)) == 0)
            return 2;
        i++;
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (argc != 3)
        return (error_print("Arguments have to be 3 (./ircserv <port> <password>)"));

    if (string_digit(argv[1]) == 1 || string_digit(argv[1]) == 2)
    {
        if (string_digit(argv[1]) == 1)
            return (error_print("Invalid port value"));
        else
            return (error_print("Invalid port size"));
    }

    if (string_chart(argv[2]) == 1 || string_chart(argv[2]) == 2)
    {
        if (string_chart(argv[2]) == 1)
            return (error_print("Invalid password size"));
        else
            return (error_print("Invalid password value"));
    }

    Server ServerObj = Server(atoi(argv[1]), argv[2]);
    ServerObj.ServerStartUp();

    return 0;
    
}
