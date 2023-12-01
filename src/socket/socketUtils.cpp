#include "socketUtils.hpp"

#include <stdlib.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int net::SocketUtils::createSocket(void)
{
    return socket(AF_INET, SOCK_STREAM, 0);
}

struct sockaddr_in* net::SocketUtils::IPv4Address(const char *ipAddress, int port)
{
    struct sockaddr_in *address = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

    if (address != nullptr) 
    {
        address->sin_family = AF_INET;
        address->sin_port = htons(port);

        if (strlen(ipAddress) == 0)
            address->sin_addr.s_addr = INADDR_ANY;
        else
            inet_pton(AF_INET, ipAddress, &address->sin_addr.s_addr);
    }

    return address;
}

int net::SocketUtils::connectToServer(int socketFileDescriptor, struct sockaddr_in *address)
{
    return connect(socketFileDescriptor, (struct sockaddr *)address, sizeof(struct sockaddr));
}