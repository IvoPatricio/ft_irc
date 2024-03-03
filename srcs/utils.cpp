#include "../includes/utils.hpp"

std::string getCmdValue(char *fullCmd)
{
    std::string cmd;
    int i = -1;
    while (fullCmd[++i] == 32 || fullCmd[i] == 9)
        continue ;
    while (fullCmd[i] != 32 && fullCmd[i] != '\n')
        i++;
    while (fullCmd[i] == 32 || fullCmd[i] == 9)
        i++;
    if (fullCmd[i] == '\0')
        return "";
    while (fullCmd[i] != '\0' && fullCmd[i] != '\n')
        cmd += fullCmd[i++];
    return cmd;
}

std::string getCmd(char *fullCmd)
{
    std::string cmd;
    int i = -1;
    while (fullCmd[++i] == 32 || fullCmd[i] == 9)
        continue ;
    if (fullCmd[i] == '\0')
        return "";
    while (fullCmd[i] != '\0' && fullCmd[i] != 32 && fullCmd[i] != '\n')
        cmd += fullCmd[i++];
    return cmd;
}

void parseMsg(std::string *msgArray, std::string fullMsg)
{
    // TODO: This logic is wrong. firstWordEnd is the beginning and not the end
    size_t firstWordBegin = fullMsg.find_first_not_of(" ");
    size_t firstWordEnd = fullMsg.find_first_of(" ", firstWordBegin);
    msgArray[0] = fullMsg.substr(firstWordBegin, firstWordEnd);
    size_t secondWordBegin = fullMsg.find_first_not_of(" ", firstWordEnd);
    msgArray[1] = fullMsg.substr(secondWordBegin);
}

int checkOneWord(std::string line)
{
    size_t firstChar = line.find_first_not_of(" ");
    size_t spaceAfterChar = line.find_first_of(" ", firstChar);
    if (line.find_first_not_of(" ", spaceAfterChar) == std::string::npos)
        return 1;
    return 0;
}