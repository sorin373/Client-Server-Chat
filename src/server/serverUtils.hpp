#pragma once
#ifndef __SERVER_UTILS_HPP__
#define __SERVER_UTILS_HPP__

#include "../socket/socketUtils.hpp"

namespace net
{
    class server
    {
    public:
        struct acceptedSocket
        {
        private:
            int acceptedSocketFileDescriptor, error;
            bool acceptStatus;
            struct sockaddr_in ipAddress;

        public:
            acceptedSocket() = default;
            
            void getAcceptedSocket(const struct sockaddr_in ipAddress, const int acceptedSocketFileDescriptor, const bool acceptStatus, const int error);
            int getAcceptedSocketFileDescriptor(void) const noexcept;
            int getError(void) const noexcept;
            bool getAcceptStatus(void) const noexcept;
            struct sockaddr_in getIpAddress(void) const noexcept;

            ~acceptedSocket() = default;
        };

    private:
        int clientSocketFileDescriptor;

    public:
        server(const int clientSocketFileDescriptor);

        void __INIT_SERVER_THREAD__(int serverSocketFileDescriptor);
        void __SERVER_THREAD__(int serverSocketFileDescriptor);
        
        int getClientSocketFileDescriptor(void) const noexcept;
        void printReceivedData(const struct acceptedSocket *socket) const noexcept;
        void acceptConnection(const int serverSocketFileDescriptor, struct acceptedSocket *__acceptedSocket);

        ~server() = default;
    };
};

#endif