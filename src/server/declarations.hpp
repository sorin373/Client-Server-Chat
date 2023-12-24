#ifndef __DECLARATIONS_HPP__
#define __DECLARATIONS_HPP__

#include <fstream>
#include "serverUtils.hpp"

extern net::server *__server;
extern std::ofstream file;

bool findString(const char haystack[], const char needle[]); // created this bec. strstr modifies the string

#endif