#pragma once
#ifndef __SERVER_UTILS_HPP__
#define __SERVER_UTILS_HPP__

#include <vector>
#include <string>
#include "../socket/socketUtils.hpp"

namespace net
{    
    class server
    {
    public:
        class database;

    public:
        template <typename S> class acceptedSocket
        {
        private:
            int                 acceptedSocketFileDescriptor, error;
            bool                acceptStatus;
            struct sockaddr_in  ipAddress;

        public:
            acceptedSocket();

            void getAcceptedSocket(const struct sockaddr_in ipAddress, const int acceptedSocketFileDescriptor, const bool acceptStatus, const int error);
            int getAcceptedSocketFileDescriptor(void) const noexcept;
            int getError(void) const noexcept;
            bool getAcceptStatus(void) const noexcept;
            struct sockaddr_in getIpAddress(void) const noexcept;

            ~acceptedSocket() = default;
        };

    private:
        int clientSocketFileDescriptor;
        std::vector<struct acceptedSocket<char>> connectedSockets;

        void handleClientConnections(int serverSocketFileDescriptor);
        template <typename T> void printReceivedDataThread(class acceptedSocket<T> *psocket);

    public:
        server(const int clientSocketFileDescriptor);

        static bool __INIT__(char *portArg = nullptr);
        void __MASTER_THREAD__(int serverSocketFileDescriptor);
        int bindServer(int serverSocketFileDescriptor, struct sockaddr_in *serverAddress);
        int getClientSocketFileDescriptor(void) const noexcept;

        template <typename T> void acceptConnection(const int serverSocketFileDescriptor, struct acceptedSocket<T> *__acceptedSocket);
        template <typename T> void sendReceivedMessage(T *buffer, int acceptedSocketFileDescriptor);
        template <typename T> void printReceivedData(class acceptedSocket<T> *socket);
        template <typename T> int handleGETrequests(T *buffer, int acceptedSocketFileDescriptor);
        template <typename T> std::vector<class acceptedSocket<T>> getConnectedSockets(void) const noexcept;

        ~server() = default;
    };
};

#endif