#ifndef __DECLARATIONS_HPP__
#define __DECLARATIONS_HPP__

#include "serverUtils.hpp"

extern net::server *__server;
extern bool DEBUG_FLAG;

bool findString(const char haystack[], const char needle[]);
bool isNumeric(const char *str);
int getMainArguments(int argc, char *argv[]);
void underline(const unsigned int vWidth);

#endif