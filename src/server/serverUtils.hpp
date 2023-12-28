#pragma once
#ifndef __SERVER_UTILS_HPP__
#define __SERVER_UTILS_HPP__

#include "interface/interface.hpp"
#include "../socket/socketUtils.hpp"

#include <vector>
#include <string>

namespace net
{    
    constexpr char BINARY_FILE_TEMP_PATH[] = "interface/storage/temp.bin";
    constexpr char LOCAL_STORAGE_PATH[] = "interface/storage/";
    constexpr char INDEX_HTML_PATH[] = "interface/index.html";

    class server
    {
    public:
        static volatile bool SERVER_RUNNING;
        class database;

    public:
        template <typename S> class acceptedSocket
        {
        private:
            struct sockaddr_in ipAddress;
            int                acceptedSocketFileDescriptor, error;
            bool               acceptStatus;

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
        std::vector<struct acceptedSocket<char>> connectedSockets;

        class database        *db;
        class interface::user *__user;

        template <typename T> void handleClientConnections(int serverSocketFileDescriptor);
        template <typename T> void receivedDataHandlerThread(class acceptedSocket<T> *psocket);
        static void consoleListener(void);
        void postRecv(const int acceptedSocketFileDescriptor);
        int formatFile(const std::string fileName);

    public:
        server(const int clientSocketFileDescriptor);

        template <typename T> void __SERVER_INIT__(int serverSocketFileDescriptor);
        int bindServer(int serverSocketFileDescriptor, struct sockaddr_in *serverAddress);
        int __database_init__(void);
        void SQLfetchUserTable(void);
        void SQLfetchFileTable(void);
        int addToFileTable(const char *fileName, const int fileSize);

        template <typename T> void acceptConnection(const int serverSocketFileDescriptor, class acceptedSocket<T> *__acceptedSocket);
        template <typename T> void receivedDataHandler(class acceptedSocket<T> *socket);
        template <typename T> int GETrequestsHandler(T *buffer, int acceptedSocketFileDescriptor);
        template <typename T> int HTTPrequestsHandler(T *buffer, int acceptedSocketFileDescriptor, ssize_t __bytesReceived);
        template <typename T> int POSTrequestsHandler(T *buffer, int acceptedSocketFileDescriptor, ssize_t __bytesReceived);

        template <typename T> std::vector<class acceptedSocket<T>> getConnectedSockets(void) const noexcept;
        int getClientSocketFileDescriptor(void) const noexcept;
        int getNoOfConnectedSockets(void) const noexcept;
        bool getServerStatus(void) const noexcept;
        class database *getSQLdatabase(void) const noexcept;
        class interface::user *getUser(void) const noexcept;

        ~server();
    };
};

#endif