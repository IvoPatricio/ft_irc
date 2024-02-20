#ifndef SIG_UTILS_HPP
# define SIG_UTILS_HPP

#include "server.hpp"

void setIsRunning(bool state);
void signal_handlers();
void doSignals(int signal);

extern bool isRunning;

#endif