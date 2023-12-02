#pragma once
#ifndef __SOCKET_UTILS_HPP__
#define __SOCKET_UTILS_HPP__

#include <netinet/in.h>

#define localHostAddress "127.0.0.1"
#define PORT 3000

namespace net
{
    class SocketUtils
    {
    public:
        SocketUtils() = default;

        int createSocket(void);
        int connectToServer(int socketFileDescriptor, struct sockaddr_in *address);
        struct sockaddr_in *IPv4Address(const char *ipAddress, int port);

        ~SocketUtils() = default;
    };
};

#endif