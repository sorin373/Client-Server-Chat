#pragma once
#ifndef __CLIENT_UTILS_HPP__
#define __CLIENT_UTILS_HPP__

#include <string>
#include <sys/socket.h>

namespace net
{
    class client 
    {
    private:
        template <typename T> void __MESSAGE_LISTENER_THREAD__(int clientSocketFileDescriptor);

    public:
        client() = default;

        static bool __INIT__(void);
        template <typename T> void __INIT_MESSAGE_LISTENER_THREAD__(int clientSocketFileDescriptor);
        template <typename T> ssize_t sendData(int socketFileDescriptor, const T *data, size_t dataSize);

        char *getClientName(size_t *__clientNameSize = nullptr);

        ~client() = default;
    };
};

#endif