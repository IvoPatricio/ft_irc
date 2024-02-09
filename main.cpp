#include <string>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <cctype>
#include "server.hpp"

int error_print(std::string error_str)
{
    std::cout << "\033[1m\033[31mERROR -\033[33m " << error_str << "\033[0m" << std::endl;
    return 0;
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
