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
 *  @brief        This C++ header file declares the 'Server' class which contains core functionalites of an HTTP Server.
 *
 */

#ifndef __SERVER_UTILS_HPP__
#define __SERVER_UTILS_HPP__

#include "interface/interface.hpp"
#include "../socket/socketUtils.hpp"

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <vector>
#include <atomic>

enum : short
{
    LENGHT = 256,
    SQL_LENGHT = 65
};

namespace net
{
    /**
     *  @brief 'temp.bin' is a file that is created when uploading a file to the Server
     *         After being written the file is correctly formated and copied. Finally temp.bin is removed.
     */
    constexpr char BINARY_FILE_TEMP_PATH[] = "interface/storage/temp.bin";
    constexpr char LOCAL_STORAGE_PATH[] = "interface/storage/";
    constexpr char INDEX_HTML_PATH[] = "interface/index.html";
    constexpr char IGNORE[] = "ignore.txt";

    /**
     * @brief This class stores all file names or extensions that are ignored when the auth checked is performed. 
     *        For example, when not logged in, the user must be able to access the login page
     *        These values are stored in the file 'ignore.txt'
     * @attention Any data added in this file will be ignored by the user auth checking system
    */
    class Ignore
    {
    public:
        class node
        {
        private:
            char *pageName;

        public:
            node *next, *prev;

            node(const char *pageName);
            char *getPageName(void) const noexcept;
            ~node();
        };

        node *head, *tail;

    public:
        Ignore();

        node *getHead(void) const noexcept;
        node *getTail(void) const noexcept;

        void fetchPages(const char *pageName);

        ~Ignore();
    };

    // Class implementing core functionalities of an HTTP Server
    template <typename T>
    class Server
    {
    public:
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

                static int getCred(char *hostname, char *username, char *password, char *database);

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
            sql::Driver *getDriver(void) const noexcept;

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
        // Class describing a network socket that has been accepted in a TCP Server
        class acceptedSocket
        {
        private:
            struct sockaddr_in ipAddress;
            int acceptedSocketFD, error;

        public:
            acceptedSocket() = default;

            void socketCleanup(void) noexcept;
            void getAcceptedSocket(const struct sockaddr_in ipAddress, const int acceptedSocketFD, const int error);
            int getAcceptedSocketFD(void) const noexcept;
            int getError(void) const noexcept;
            struct sockaddr_in getIpAddress(void) const noexcept;

            ~acceptedSocket() = default;
        };

    private:
        static std::atomic<bool> SERVER_RUNNING;

        Ignore ignore;                                      // double linked list which contains pages and file extensions that must be ignore when checking for page authorization

        std::vector<class acceptedSocket> connectedSockets; // Vector that stores all the connected sockets.
        class interface::User *user;                        // Pointer to the 'user' object.
        class db              *db;                          // Pointer to the 'database' object.

        /**
         * @brief This function handles client connections. It creates a new 'acceptedSocket' object for every incoming connection using the new operator.
         * @param acceptedSocketFD The file descriptor for the accepted socket connection used when seding the HTTP response.
         */
        void handleClientConnections(int serverSocketFD, std::atomic<bool> &server_running);

        /**
         * @brief This function crestes a thread for an accepted socket. It creates a new 'acceptedSocket' object for every incoming connection using the new operator.
         * @param acceptedSocket Object describing a network socket that has been accepted in a TCP Server.
         */
        void receivedDataHandlerThread(const class acceptedSocket socket);

        /**
         * @brief If a file has been uploaded to the Server this function performs post receive file formating on the 'temp.bin' file.
         * @return Returns 0 on success, 1 for errors.
         */
        int formatFile(const std::string fileName);

        /**
         * @brief If a file has been uploaded to the Server this function performs post receive operations such as:
         *        file formatting, adding the file metadata to the database and clearing the file from the queue.
         * @param acceptedSocketFD The file descriptor for the accepted socket connection used when seding the HTTP response.
         */
        void postRecv(const int acceptedSocketFD);

        void consoleListener(std::atomic<bool> &server_running);

    public:
        Server();

        inline int read_ignore_data(void);

        /*
         * This function does the following:
         *   - Sets the Server status as running
         *   - Deletes content written in index.html
         *   - Creates a detached thread that handles client connections
         *   - Creates a thread that listens for console input
         */
        void server_easy_init(int serverSocketFD);

        /*
         * This function binds a socket to a specific address and port. Return 0 for success, -1 for errors:
         *   - Default address: 127.0.0.1
         *   - Default port:    8080
         */
        int bindServer(int serverSocketFD, struct sockaddr_in *serverAddress);

        /*
         * This function gets the database credentials and the establishes connection. Returns 0 on success, 1 for errors.
         *   - It allocates memory for the 'database' object class using the new operator (memory release is handled automatically)
         *   - It allocates memory for the 'user' object class using the new operator (memory release is handled automatically)
         */
        int database_easy_init(void);

        // This function retrieves the "user" table from the database
        void SQLfetchUserTable(void);

        // This function retrieves the "file" table from the database
        void SQLfetchFileTable(void);

        /**
         * @brief This function adds the files uploaded to the Server.
         * @return Returns 0 on success, 1 for errors.
         */
        int addToFileTable(const char *fileName, const int fileSize);

        // This function accepts client connections.
        bool acceptConnection(const int serverSocketFD, class acceptedSocket &acceptedSocket);

        // This function receives the data sent by a client.
        void receivedDataHandler(const class acceptedSocket socket);

        /**
         * @brief This function handles HTTP POST requests.
         *
         * @param buffer Contains the request data.
         * @param acceptedSocketFD The file descriptor for the accepted socket connection used when seding the HTTP response.
         * @param bytesReceived The size of the current buffer
         *
         * @return Returns 0 on success, 1 for errors.
         */
        int POSTrequestsHandler(T *buffer, int acceptedSocketFD, ssize_t bytesReceived);

        /**
         * @brief This function handles HTTP GET requests.
         *
         * @param buffer Contains the request data.
         * @param acceptedSocketFD The file descriptor for the accepted socket connection used when seding the HTTP response.
         *
         * @return Returns 0 on success, 1 for errors.
         */
        int GETrequestsHandler(T *buffer, int acceptedSocketFD);

        /**
         * @brief This function decides whether the HTTP request is a POST or GET request.
         *
         * @param buffer Contains the request data.
         * @param acceptedSocketFD The file descriptor for the accepted socket connection used when seding the HTTP response.
         * @param bytesReceived The size of the current buffer
         *
         * @return Returns 0 on success, 1 for errors.
         */
        int HTTPrequestsHandler(T *buffer, int acceptedSocketFD, ssize_t bytesReceived);

        // This function retrieves a vector where the connected clients are stored
        std::vector<class acceptedSocket> getConnectedSockets(void) const noexcept;

        // This function retrieves the status of the Server. Returns true if the Server is running, otherwise false.
        bool getServerStatus(void) const noexcept;

        /**
         *  @brief This function retrives the ignore class variable. It stores the file names and extensions which shall be ignored by the user auth check
         *  @attention Check class documentation for more information
         */
        Ignore getIgnore(void) const noexcept;
        
        // This function changes the value of the SERVER_RUNNING var, ultimately stopping the server
        void haltServer(void) noexcept;

        // This function retrieves a pointer to the "database" object
        class db *getSQLdatabase(void) const noexcept;

        // This function retrieves a pointer to the "user" object
        class interface::User *getUser(void) const noexcept;

        ~Server();
    };

    int INIT(int argc, char *argv[]);
};

#endif // __SERVER_UTILS_HPP__