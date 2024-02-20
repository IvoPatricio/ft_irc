#include "../includes/sig_utils.hpp"

bool isRunning = true;

void setIsRunning(bool state)
{
    isRunning = state;
}

void doSignals(int signal)
{
    if (signal == SIGINT) 
    {
        std::cout << "Received SIGNAL to terminate. Shutting down the server" << std::endl;
        setIsRunning(false);
    }
    else if (signal == SIGQUIT)
    {
        std::cout << "Received SIGNAL to terminate forcefully. Shutting down the server" << std::endl;
        setIsRunning(false);
        //forcing core dump
        abort();
    }
}

void signal_handlers()
{
    struct sigaction siga;
    siga.sa_handler = doSignals;
    siga.sa_flags = 0;

    //update signals values when clients leave
    sigaction(SIGINT, &siga, NULL);
    sigaction(SIGQUIT, &siga, NULL);
}