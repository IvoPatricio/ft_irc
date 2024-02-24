#include "../includes/main.hpp"

std::string extractAfterCmd(std::string cmd)
{
    size_t firstWord = cmd.find_first_not_of(" ");
    if (firstWord == std::string::npos)
        return "";
    size_t secondWord = cmd.find_first_of(" ", firstWord + 1);
    return cmd.substr(secondWord);
}

int checkOneWord(std::string line)
{
    size_t firstChar = line.find_first_not_of(" ");
    if (firstChar == std::string::npos)
        return 0;
    size_t lastChar = line.find(" ", firstChar + 1);
    if (line.find_first_not_of(" ", lastChar + 1) == std::string::npos)
        return 1;
    return 0;
}

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

    if (port.size() > 5 || atoi(port.c_str()) <= 1023)
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

    if (string_digit(argv[1]) != 0)
    {
        if (string_digit(argv[1]) == 1)
            return (error_print("Invalid port value"));
        else
            return (error_print("Invalid port size"));
    }

    if (string_chart(argv[2]) != 0)
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
