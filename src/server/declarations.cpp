#include "declarations.hpp"

#include <iostream>
#include <cstring>
#include <cctype>

net::server *__server = nullptr;

bool findString(const char haystack[], const char needle[]) // created this bec. strstr modifies the string
{
    char *__copyHaystack = new char[strlen(haystack) + 1];
    strcpy(__copyHaystack, haystack);

    if (strstr(__copyHaystack, needle) != NULL)
    {
        delete[] __copyHaystack;
        return true;
    }
    
    delete[] __copyHaystack;
    return false;
}

bool isNumeric(const char *str)
{
    for (unsigned int i = 0, n = strlen(str); i < n; i++)
        if (!std::isdigit(str[i]))
            return false;

    return true;
}