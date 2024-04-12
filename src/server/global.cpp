#include "global.hpp"

#include <iostream>
#include <cstring>
#include <iomanip>
#include <termios.h>

net::Server<char> *server = nullptr;

bool DEBUG_FLAG = false;

double TOTAL_BYTES_RECV = 0;

bool findString(const char haystack[], const char needle[])
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
        if (!std::isdigit(str[i])) return false;

    return true;
}

int getMainArguments(int argc, char *argv[])
{
    int port = 0;

    if (argc > 3)
    {
        std::cerr << "\n"
                  << std::setw(5) << " "
                  << "--> Invalid number of arguments provided.\n\n"
                  << std::setw(5) << " "
                  << "--> Usage: \n"
                  << std::setw(5) << " "
                  << "       ./httpServer [port] [-debug]\n"
                  << std::setw(5) << " "
                  << "       ./httpServer [-debug]\n"
                  << std::setw(5) << " "
                  << "       ./httpServer [port]\n\n"
                  << std::setw(5) << " "
                  << "--> Optional flags:\n"
                  << std::setw(5) << " "
                  << "       [-debug]       Enable debug mode to display HTTP requests.\n"
                  << std::setw(5) << " "
                  << "       [port]         Specify a particular port number for the Server to listen on.\n\n"
                  << std::setw(5) << " "
                  << "--> If no port is provided, the default port will be used.\n\n";

        return -1;
    }

    switch (argc)
    {
    case 1:
        port = net::DEFAULT_PORT;
        break;
    case 2:
        if (isNumeric(argv[1]))
            port = atoi(argv[1]);

        if (port == 0)
            if (strcmp(argv[1], "-debug") == 0)
            {
                DEBUG_FLAG = true;
                port = net::DEFAULT_PORT;
            }

        break;
    case 3:
        if (isNumeric(argv[1]))
            port = atoi(argv[1]);

        if (argv[2] != nullptr)
        {
            if (strcmp(argv[2], "-debug") == 0)
            {
                DEBUG_FLAG = true;

                if (port == 0)
                {
                    std::cerr << "\n"
                              << std::setw(5) << " "
                              << "--> Invalid port number provided. Please use a valid port number (e.g., 5000).\n\n"
                              << std::setw(5) << " "
                              << "--> Usage: \n"
                              << std::setw(5) << " "
                              << "       ./httpServer [port] [-debug]\n"
                              << std::setw(5) << " "
                              << "       ./httpServer [-debug]\n"
                              << std::setw(5) << " "
                              << "       ./httpServer [port]\n\n"
                              << std::setw(5) << " "
                              << "--> Optional flags:\n"
                              << std::setw(5) << " "
                              << "       [-debug]       Enable debug mode to display HTTP requests.\n"
                              << std::setw(5) << " "
                              << "       [port]         Specify a particular port number for the Server to listen on.\n\n"
                              << std::setw(5) << " "
                              << "--> If no port is provided, the default port will be used.\n\n";

                    return -1;
                }
            }
            else if (port != 0)
            {
                std::cerr << "\n"
                          << std::setw(5) << " "
                          << "--> Invalid flag provided. Please use a valid flag (e.g., -debug).\n\n"
                          << std::setw(5) << " "
                          << "--> Usage: \n"
                          << std::setw(5) << " "
                          << "       ./httpServer [port] [-debug]\n"
                          << std::setw(5) << " "
                          << "       ./httpServer [-debug]\n"
                          << std::setw(5) << " "
                          << "       ./httpServer [port]\n\n"
                          << std::setw(5) << " "
                          << "--> Optional flags:\n"
                          << std::setw(5) << " "
                          << "       [-debug]       Enable debug mode to display HTTP requests.\n"
                          << std::setw(5) << " "
                          << "       [port]         Specify a particular port number for the Server to listen on.\n\n"
                          << std::setw(5) << " "
                          << "--> If no port is provided, the default port will be used.\n\n";

                return -1;
            }
            else
            {
                std::cerr << "\n"
                          << std::setw(5) << " "
                          << "--> Invalid arguments provided. Please use a valid flag (e.g., -debug) or port number (e.g., 5000).\n\n"
                          << std::setw(5) << " "
                          << "--> Usage: \n"
                          << std::setw(5) << " "
                          << "       ./httpServer [port] [-debug]\n"
                          << std::setw(5) << " "
                          << "       ./httpServer [-debug]\n"
                          << std::setw(5) << " "
                          << "       ./httpServer [port]\n\n"
                          << std::setw(5) << " "
                          << "--> Optional flags:\n"
                          << std::setw(5) << " "
                          << "       [-debug]       Enable debug mode to display HTTP requests.\n"
                          << std::setw(5) << " "
                          << "       [port]         Specify a particular port number for the Server to listen on.\n\n"
                          << std::setw(5) << " "
                          << "--> If no port is provided, the default port will be used.\n\n";

                return -1;
            }
        }

        break;
    }

    if (port == 0)
    {
        std::cerr << "\n"
                  << std::setw(5) << " "
                  << "--> Invalid arguments provided. Please use a valid flag (e.g., -debug) or port number (e.g., 5000).\n\n"
                  << std::setw(5) << " "
                  << "--> Usage: \n"
                  << std::setw(5) << " "
                  << "       ./httpServer [port] [-debug]\n"
                  << std::setw(5) << " "
                  << "       ./httpServer [-debug]\n"
                  << std::setw(5) << " "
                  << "       ./httpServer [port]\n\n"
                  << std::setw(5) << " "
                  << "--> Optional flags:\n"
                  << std::setw(5) << " "
                  << "       [-debug]       Enable debug mode to display HTTP requests.\n"
                  << std::setw(5) << " "
                  << "       [port]         Specify a particular port number for the Server to listen on.\n\n"
                  << std::setw(5) << " "
                  << "--> If no port is provided, the default port will be used.\n\n";

        return -1;
    }

    return port;
}

void underline(const unsigned int vWidth)
{
    std::cout << std::setw(5 - 2) << " ";
    char fillLine;
    fillLine = std::cout.fill('_');
    std::cout.width(vWidth);
    std::cout << '_' << "\n";
    std::cout.fill(fillLine);
    std::cout << "\n";
}

void toggleEcho(bool enable)
{
    struct termios settings;
    tcgetattr(STDIN_FILENO, &settings);

    if (!enable)
        // Disable echo
        settings.c_lflag &= ~ECHO;
    else
        // Enable echo
        settings.c_lflag |= ECHO;

    tcsetattr(STDIN_FILENO, TCSANOW, &settings);
}