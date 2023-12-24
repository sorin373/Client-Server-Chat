#include "declarations.hpp"

#include <cstring>

net::server *__server = nullptr;
std::ofstream file;

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