#pragma once
#ifndef __SERVER_UTILS_HPP__
#define __SERVER_UTILS_HPP__

#include <vector>
#include "../socket/socketUtils.hpp"

namespace net
{
    class server
    {
    public:
        class acceptedSocket
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
        std::vector<struct acceptedSocket> connectedSockets;

    private:
        void handleClientConnections(int serverSocketFileDescriptor);
        void printReceivedDataThread(struct acceptedSocket *psocket);

    public:
        server(const int clientSocketFileDescriptor);

        static bool __INIT__(void);
        void __MASTER_THREAD__(int serverSocketFileDescriptor);
        void acceptConnection(const int serverSocketFileDescriptor, struct acceptedSocket *__acceptedSocket);
        int bindServer(int serverSocketFileDescriptor, struct sockaddr_in *serverAddress);

        template <typename T> void sendReceivedMessage(T *buffer, int acceptedSocketFileDescriptor);
        template <typename T> void printReceivedData(const struct acceptedSocket *socket);

        int getClientSocketFileDescriptor(void) const noexcept;
        std::vector<struct acceptedSocket> getConnectedSockets(void) const noexcept;

        ~server() = default;
    };
};

#endif