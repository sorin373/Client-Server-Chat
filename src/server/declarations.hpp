#ifndef __DECLARATIONS_HPP__
#define __DECLARATIONS_HPP__

#include "serverUtils.hpp"

extern net::server *__server;

bool findString(const char haystack[], const char needle[]);
bool isNumeric(const char *str);

#endif