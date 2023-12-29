/**
 *
 *  @file         serverUtils.hpp
 *
 *  @copyright    MIT License
 *
 *                Copyright (c) 2023 Sorin Tudose
 *
 *                Permission is hereby granted, free of charge, to any person obtaining a copy
 *                of this software and associated documentation files (the "Software"), to deal
 *                in the Software without restriction, including without limitation the rights
 *                to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *                copies of the Software, and to permit persons to whom the Software is
 *                furnished to do so, subject to the following conditions:
 *
 *                The above copyright notice and this permission notice shall be included in all
 *                copies or substantial portions of the Software.
 *
 *                THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *                IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *                FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *                AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *                LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *                OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *                SOFTWARE.
 *
 *  @brief        This C++ header file declares the 'server' class which contains core functionalites of an HTTP server.
 *
 */

#ifndef __SERVER_UTILS_HPP__
#define __SERVER_UTILS_HPP__

#include "interface/interface.hpp"
#include "../socket/socketUtils.hpp"

#include <vector>
#include <string>

namespace net
{
    constexpr char BINARY_FILE_TEMP_PATH[] = "interface/storage/temp.bin"; // temp.bin is a file that is created when uploading a file to the server
                                                                           // After being written the file is correctly formated and copied
                                                                           // Finally temp.bin is removed
    constexpr char LOCAL_STORAGE_PATH[] = "interface/storage/";
    constexpr char INDEX_HTML_PATH[] = "interface/index.html";

    // Class implementing core functionalities of an HTTP server
    template <typename T>
    class server
    {
    public:
        static volatile bool SERVER_RUNNING;
        // Forward declaration for the class implementing core functionalities of database. However the tables are stored in the interface class.
        class database;

    public:
        // Class describing a network socket that has been accepted in a TCP server
        class acceptedSocket
        {
        private:
            struct sockaddr_in ipAddress;
            int acceptedSocketFileDescriptor, error;

        public:
            acceptedSocket() = default;

            void getAcceptedSocket(const struct sockaddr_in ipAddress, const int acceptedSocketFileDescriptor, const int error);
            int getAcceptedSocketFileDescriptor(void) const noexcept;
            int getError(void) const noexcept;
            struct sockaddr_in getIpAddress(void) const noexcept;

            ~acceptedSocket() = default;
        };

    private:
        std::vector<struct acceptedSocket> connectedSockets; // Vector that stores all the connected sockets.

        class database *db;            // Pointer to the database object.
        class interface::user *__user; // Pointer to the user object.

        void handleClientConnections(int serverSocketFileDescriptor);
        void receivedDataHandlerThread(class acceptedSocket *socket);
        static void consoleListener(void);
        void postRecv(const int acceptedSocketFileDescriptor);
        int formatFile(const std::string fileName);

    public:
        server();

        /*
         * This generic function does the following:
         *   - Sets the server status as running
         *   - Deletes content written in index.html
         *   - Creates a detached thread that handles client connections
         *   - Creates a thread that listens for console input
         */
        void __SERVER_INIT__(int serverSocketFileDescriptor);

        /*
         * This function binds a socket to a specific address and port. Return 0 for success, -1 for errors:
         *   - Default address: 127.0.0.1
         *   - Default port:    8080
         */
        int bindServer(int serverSocketFileDescriptor, struct sockaddr_in *serverAddress);

        /*
         * This function gets the database credentials and the establishes connection. Returns 0 on success, 1 for errors.
         *   - It allocates memory for the 'database' object class using the new operator (memory release is handled automatically)
         *   - It allocates memory for the 'user' object class using the new operator (memory release is handled automatically)
         */
        int __database_init__(void);

        // This function retrieves the "user" table from the database
        void SQLfetchUserTable(void);

        // This function retrieves the "file" table from the database
        void SQLfetchFileTable(void);

        /**
         * @brief This function adds the files uploaded to the server.
         * @return Returns 0 on success, 1 for errors.
         */
        int addToFileTable(const char *fileName, const int fileSize);

        // This function accepts client connections.
        void acceptConnection(const int serverSocketFileDescriptor, class acceptedSocket *__acceptedSocket);

        // This function receives the data sent by a client.
        void receivedDataHandler(class acceptedSocket *socket);

        /**
         * @brief This function handles HTTP POST requests.
         * @return Returns 0 on success, 1 for errors.
         */
        int POSTrequestsHandler(T *buffer, int acceptedSocketFileDescriptor, ssize_t __bytesReceived);

        /**
         * @brief This function handles HTTP GET requests.
         * @return Returns 0 on success, 1 for errors.
         */
        int GETrequestsHandler(T *buffer, int acceptedSocketFileDescriptor);

        /**
         * @brief This function decides whether the HTTP request is a POST or GET request.
         * @return Returns 0 on success, 1 for errors.
         */
        int HTTPrequestsHandler(T *buffer, int acceptedSocketFileDescriptor, ssize_t __bytesReceived);

        // This function retrieves a vector where the connected clients are stored
        std::vector<class acceptedSocket> getConnectedSockets(void) const noexcept;

        // This function retrieves the status of the server. Returns true if the server is running, otherwise false.
        bool getServerStatus(void) const noexcept;

        // This function retrieves a pointer to the "database" object
        class database *getSQLdatabase(void) const noexcept;

        // This function retrieves a pointer to the "user" object
        class interface::user *getUser(void) const noexcept;

        ~server();
    };
};

#endif // __SERVER_UTILS_HPP__