#pragma once
#ifndef __SOCKET_UTILS_HPP__
#define __SOCKET_UTILS_HPP__

#include <netinet/in.h>

namespace net
{
    class SocketUtils
    {
    public:
        SocketUtils();

        int createSocket(void);
        int connectToServer(int socketFileDescriptor, struct sockaddr_in *address);
        struct sockaddr_in *IPv4Address(const char *ipAddress, int port);

        ~SocketUtils();
        
    };
};

#endif