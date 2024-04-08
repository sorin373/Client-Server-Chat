#include "socketUtils.hpp"

#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace net;

static char *exec(const char *cmd)
{
    FILE *pipe = popen(cmd, "r");

    if (!pipe)
    {
        std::cerr << "Could not execute the command: " << cmd << "\n";
        return nullptr;
    }

    char buffer[128];
    char *result = new char[4096];
    size_t totalSize = 0;

    while (!feof(pipe))
    {
        if (fgets(buffer, 128, pipe) != nullptr)
        {
            size_t len = strlen(buffer);
            strncpy(result + totalSize, buffer, len);
            totalSize += len;
        }
    }

    pclose(pipe);

    return result;
}

int SocketUtils::createSocket(int addressFamily, int socketType, int protocol)
{
    return socket(addressFamily, socketType, protocol);
}

struct sockaddr_in *SocketUtils::IPv4Address(const char *ipAddress, int port)
{
    struct sockaddr_in *address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

    if (address == nullptr) return nullptr;

    if (address != nullptr)
    {
        address->sin_family = AF_INET;
        address->sin_port = htons(port);

        if (strlen(ipAddress) == 0 || strcasecmp(ipAddress, "INADDR_ANY") == 0 || ipAddress == nullptr) address->sin_addr.s_addr = INADDR_ANY;
        else inet_pton(AF_INET, ipAddress, &address->sin_addr.s_addr);
    }

    return address;
}

int SocketUtils::connectToServer(int socketFileDescriptor, struct sockaddr_in *address)
{
    return connect(socketFileDescriptor, (struct sockaddr *)address, sizeof(struct sockaddr));
}

char *SocketUtils::getMachineIPv4Address(void)
{
    char *ifconfigOutput = exec("ifconfig");

    if (ifconfigOutput == nullptr)
        return nullptr;

    char *pos = strstr(ifconfigOutput, "inet ");

    if (pos != nullptr)
    {
        pos = strstr(pos + 1, "inet ");

        if (pos != nullptr)
        {
            pos += 5;
            char *end = strchr(pos, ' ');

            if (end != nullptr)
            {
                size_t len = end - pos;
                char *address = new char[len + 1];
                strncpy(address, pos, len);
                address[len] = '\0';
                
                delete[] ifconfigOutput;

                return address;
            }
        }
    }

    delete[] ifconfigOutput;

    return nullptr;
}

void SocketUtils::closeSocket(int socketFileDescriptor)
{ 
    close(socketFileDescriptor);
}