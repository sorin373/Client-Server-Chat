#pragma once
#ifndef __SOCKET_UTILS_HPP__
#define __SOCKET_UTILS_HPP__

#include <netinet/in.h>

namespace net
{
    constexpr char localHostAddress[] = "127.0.0.1";
    constexpr int PORT = 8080;

    class SocketUtils
    {
    public:
        SocketUtils() = default;
        
        static char *getMachineIPv4Address(void);
        int createSocket(void);
        int connectToServer(int socketFileDescriptor, struct sockaddr_in *address);
        struct sockaddr_in *IPv4Address(const char *ipAddress = localHostAddress, int port = PORT);

        ~SocketUtils() = default;
    };
};

#endif