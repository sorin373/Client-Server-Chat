#pragma once
#ifndef __CLIENT_UTILS_HPP__
#define __CLIENT_UTILS_HPP__

#include <string>

namespace net
{
    class client 
    {
    private:
        void __MESSAGE_LISTENER_THREAD__(int clientSocketFileDescriptor);

    public:
        client() = default;

        void __INIT_MESSAGE_LISTENER_THREAD__(int clientSocketFileDescriptor);
        char *getClientName(size_t *__clientNameSize = nullptr);

        template <typename T> 
        ssize_t sendData(int socketFileDescriptor, const T *data, size_t dataSize)
        {
            return send(socketFileDescriptor, data, dataSize * sizeof(T), 0);
        }

        ~client() = default;
    };
};

#endif