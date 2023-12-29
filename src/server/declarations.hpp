#ifndef __DECLARATIONS_HPP__
#define __DECLARATIONS_HPP__

#include "serverUtils.hpp"

// Extern pointer to the instance of the net::server class. This allows all files that include this header file to have access to this variable
extern net::server *__server;

extern bool DEBUG_FLAG;

// This function searches for a substring. Returns true if the string is found, otherwise false.
bool findString(const char haystack[], const char needle[]);

// This function checks if a string is a number by checking if all characters are digits. Returns true if the string is found, otherwise false.
bool isNumeric(const char *str);

// This function is called at the begining of the main function. It checks if the argumets provided by the user are valid and what is the port value
int getMainArguments(int argc, char *argv[]);

// This function is designed to generate an underline of a specified width 
void underline(const unsigned int vWidth);

#endif //__DECLARATIONS_HPP__