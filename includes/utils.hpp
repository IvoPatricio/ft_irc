#ifndef UTILS_HPP
# define UTILS_HPP

# include "main.hpp"
# include <algorithm>

std::string getCmdValue(char *fullCmd);
std::string getCmd(char *fullCmd);
void parseMsg(std::string *msgArray, std::string fullMsg);
int checkOneWord(std::string line);

#endif