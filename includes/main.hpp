#ifndef MAIN_HPP
# define MAIN_HPP

#include <string>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <stdio.h>
#include "server.hpp"

#define RED "\033[1m\033[31m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
#define GREEN "\033[1;32m"

int success_print(std::string success_str);
int string_digit(std::string port);
int string_chart(std::string pass);
int error_print(std::string error_str);

#endif
