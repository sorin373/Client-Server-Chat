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
#include <mysql_connection.h>
#include <mysql_driver.h>

#define LENGHT     256
#define SQL_LENGHT 65

namespace net
{
    /**
     *  @brief 'temp.bin' is a file that is created when uploading a file to the server
     *         After being written the file is correctly formated and copied. Finally temp.bin is removed.
     */
    constexpr char BINARY_FILE_TEMP_PATH[] = "interface/storage/temp.bin";
    constexpr char LOCAL_STORAGE_PATH[] = "interface/storage/";
    constexpr char INDEX_HTML_PATH[] = "interface/index.html";

    // Class implementing core functionalities of an HTTP server
    template <typename T>
    class server
    {
    public:
        static volatile bool SERVER_RUNNING;

        // Forward declaration for the class implementing core functionalities of database. However the tables are stored in the interface class.
        class db
        {
        public:
            // Nested class describing the database credentials object.
            class db_cred
            {
            private:
                char *hostname, *username, *password, *database;

            public:
                db_cred(const char *hostname, const char *username, const char *password, const char *database);

                char *getHostname(void) const noexcept;
                char *getUsername(void) const noexcept;
                char *getPassword(void) const noexcept;
                char *getDatabase(void) const noexcept;

                static int getCred(const char *hostname, const char *username, const char *password, const char *database);

                ~db_cred();
            };
        
        private:
            class db_cred   *dbCred; // Pointer to the 'db_cred' object.
            sql::Connection *con;
            sql::Driver     *driver;
        
        public:
            db(sql::Driver *driver, sql::Connection *con, const char *hostname, const char *username, const char *password, const char *database);
        
            /**
             * @brief This function retrieves the SQL driver.
             * @return Returns a pointer to the SQL driver object.
             */
            sql::Driver *getDriver(void)  const noexcept;

            /**
             * @brief This function retrieves the database connection.
             * @return Returns a pointer to the SQL connection object.
             */
            sql::Connection *getCon(void) const noexcept;

            /**
             * @brief This function retrieves the database credentials.
             * @return Returns a pointer to the database credentials object.
             */
            class db_cred *getDB_Cred(void) const noexcept;

            ~db();
        };

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
        class interface::user *__user;                       // Pointer to the 'user' object.
        class db              *db;                           // Pointer to the 'database' object.
        
        
        /**
         * @brief This function handles client connections. It creates a new 'acceptedSocket' object for every incoming connection using the new operator.
         * @param acceptedSocketFileDescriptor The file descriptor for the accepted socket connection used when seding the HTTP response.
         */
        void handleClientConnections(int serverSocketFileDescriptor);

        /**
         * @brief This function crestes a thread for an accepted socket. It creates a new 'acceptedSocket' object for every incoming connection using the new operator.
         * @param acceptedSocket Object describing a network socket that has been accepted in a TCP server.
         */
        void receivedDataHandlerThread(class acceptedSocket socket);

        /**
         * @brief If a file has been uploaded to the server this function performs post receive file formating on the 'temp.bin' file. 
         * @return Returns 0 on success, 1 for errors.
         */
        int formatFile(const std::string fileName);
        
        /**
         * @brief If a file has been uploaded to the server this function performs post receive operations such as: 
         *        file formatting, adding the file metadata to the database and clearing the file from the queue.
         * @param acceptedSocketFileDescriptor The file descriptor for the accepted socket connection used when seding the HTTP response.
         */
        void postRecv(const int acceptedSocketFileDescriptor);

        static void consoleListener(void);

    public:
        server();

        /*
         * This function does the following:
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
        void acceptConnection(const int serverSocketFileDescriptor, class acceptedSocket &__acceptedSocket);

        // This function receives the data sent by a client.
        void receivedDataHandler(const class acceptedSocket socket);

        /**
         * @brief This function handles HTTP POST requests.
         *
         * @param buffer Contains the request data.
         * @param acceptedSocketFileDescriptor The file descriptor for the accepted socket connection used when seding the HTTP response.
         * @param __bytesReceived The size of the current buffer
         *
         * @return Returns 0 on success, 1 for errors.
         */
        int POSTrequestsHandler(T *buffer, int acceptedSocketFileDescriptor, ssize_t __bytesReceived);

        /**
         * @brief This function handles HTTP GET requests.
         *
         * @param buffer Contains the request data.
         * @param acceptedSocketFileDescriptor The file descriptor for the accepted socket connection used when seding the HTTP response.
         *
         * @return Returns 0 on success, 1 for errors.
         */
        int GETrequestsHandler(T *buffer, int acceptedSocketFileDescriptor);

        /**
         * @brief This function decides whether the HTTP request is a POST or GET request.
         *
         * @param buffer Contains the request data.
         * @param acceptedSocketFileDescriptor The file descriptor for the accepted socket connection used when seding the HTTP response.
         * @param __bytesReceived The size of the current buffer
         *
         * @return Returns 0 on success, 1 for errors.
         */
        int HTTPrequestsHandler(T *buffer, int acceptedSocketFileDescriptor, ssize_t __bytesReceived);

        // This function retrieves a vector where the connected clients are stored
        std::vector<class acceptedSocket> getConnectedSockets(void) const noexcept;

        // This function retrieves the status of the server. Returns true if the server is running, otherwise false.
        bool getServerStatus(void) const noexcept;

        // This function retrieves a pointer to the "database" object
        class db *getSQLdatabase(void) const noexcept;

        // This function retrieves a pointer to the "user" object
        class interface::user *getUser(void) const noexcept;

        ~server();
    };
};

#endif // __SERVER_UTILS_HPP__