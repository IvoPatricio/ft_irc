#include "../includes/utils.hpp"

int	sendIrcMessage(std::string message, int clientId)
{
	message = message + "\r\n";
	std::cout << "Sending message to clt id " << clientId << " : " << message << std::endl;
	if (send(clientId, message.c_str(), message.length(), 0) == -1)
		exit(error_print("Error sending message"));
    message.clear();
	return 0;
}

std::string getCmdValue(char *fullCmd)
{
    std::string cmd;
    int i = -1;
    while (fullCmd[++i] == 32 || fullCmd[i] == 9 || fullCmd[i] == 13)
        continue ;
    while (fullCmd[i] != 32 && fullCmd[i] != '\n' && fullCmd[i] != '\0')
        i++;
    while (fullCmd[i] == 32 || fullCmd[i] == 9 || fullCmd[i] == 13)
        i++;
    if (fullCmd[i] == '\0')
        return "";
    while (fullCmd[i] != '\0' && fullCmd[i] != '\n')
    {
        if (fullCmd[i] == '\n' && fullCmd[i + 1] == '\0')
            return cmd;
        if (fullCmd[i] == 13)
        {
            i++;
            continue ;
        }
        cmd += fullCmd[i++];
    }
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
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::toupper);
    return cmd;
}

void parseMsg(std::string *msgArray, std::string fullMsg)
{
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
    if (spaceAfterChar == std::string::npos || line.find_first_not_of(" ", spaceAfterChar) == std::string::npos)
        return 1;
    return 0;
}