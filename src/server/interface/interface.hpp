/**
 *
 *  @file         interface.hpp
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
 *  @brief        This C++ header file provides core functionalities for user-related Server functionalities.
 *
 */

#pragma once

#include <vector>
#include <string>
#include <stdlib.h>
#include <netinet/in.h>

#include "../tcp_listener.hpp"
#include "../back-end/database.hpp"

#define NET_USERNAME_LENGHT 32
#define NET_PASSWORD_LENGHT 64

namespace net
{
    namespace interface
    {
        struct user_credentials
        {
            user_credentials() noexcept
                : m_username(nullptr), m_password(nullptr), m_id() {}

            explicit user_credentials(const char *username, const char *password, const int id)
                : m_username(nullptr), m_password(nullptr), m_id(id)
            {
                if (username == nullptr || password == nullptr)
                    throw std::invalid_argument("Table field is nullptr\n");

                this->m_username = strdup(username);
                this->m_password = strdup(password);
            }

            user_credentials(const user_credentials &other)
                : user_credentials(other.m_username, other.m_password, other.m_id) { }

            user_credentials& operator=(const user_credentials&) = delete;

            ~user_credentials()
            {
                if (this->m_username != nullptr)
                {
                    free(this->m_username);
                    this->m_username = nullptr;
                }

                if (this->m_password != nullptr)
                {
                    free(this->m_password);
                    this->m_password = nullptr;
                }
            }

            const char *password() const noexcept { return this->m_password; }

            const char *username() const noexcept { return this->m_username; }

            const int id() const noexcept { return this->m_id; }

        private:
            char *m_username, *m_password;
            int m_id;
        };

        struct user_files
        {
            user_files() noexcept
                : m_file_name(nullptr), m_date(nullptr), m_userID(), m_fileID(), m_file_size() {}

            user_files(const char *file_name, const int userID, const int fileID, const double file_size, const char *date)
                : m_file_name(nullptr), m_date(nullptr), m_userID(userID), m_fileID(fileID), m_file_size(file_size)
            {
                if (file_name == nullptr || date == nullptr)
                    throw std::invalid_argument("Table field is nullptr\n");

                this->m_file_name = strdup(file_name);
                this->m_date = strdup(date);
            }

            user_files(const user_files &other)
                : user_files(other.m_file_name, other.m_userID, other.m_fileID, other.m_file_size, other.m_date) { }

            user_files& operator=(const user_files&) = delete;

            const char *file_name() const noexcept { return this->m_file_name; }

            const char *date() const noexcept { return this->m_date; }

            const int userID() const noexcept { return this->m_userID; }

            const int fileID() const noexcept { return this->m_fileID; }

            const double file_size() const noexcept { return this->m_file_size; }

        private:
            char *m_file_name, *m_date;
            int m_userID, m_fileID;
            double m_file_size;
        };

        // Class describing a 'user' and it is part of the Server's interface namespace.
        class user : private tcp_listener
        {
        public:
            user() 
                : AUTH_STATUS(false), SESSION_ID(-1)
            { this->m_mysql_handle = static_cast<MySQL_Handle*>(malloc(sizeof(MySQL_Handle))); }

            ~user()
            {
                this->uc.clear();
                this->uf.clear();

                free(this->m_mysql_handle);
            }

            MySQL_Handle *mysql_handler() const noexcept
            { return this->m_mysql_handle; }

            int routeManager(void *buffer, char *route, int acceptedSocketFD, ssize_t bytesReceived);

            // This function retrieves a vector where the user credentials are stored
            std::vector<class user_credentials> getUserCredentials(void) const noexcept
            { return this->uc; }

            // This function retrieves a vector where the user files are stored
            std::vector<class user_files> getUserFiles(void) const noexcept
            { return this->uf; }

            // This function retrieves a string that holdes the name of the file that needs to be inserted in the database
            std::string getFileInQueue() const noexcept
            { return this->fileInQueue; }

            // This function retrieves the session ID
            int getSessionID(void) const noexcept
            { return this->SESSION_ID; }

            /**
             * @brief This function retrieves the authentification status.
             * @return Returns true if the user is signed in, false otherwise.
             */
            bool getAuthStatus(void) const noexcept
            { return this->AUTH_STATUS; }

            // This function is used to reset the variable that is used to check if the user is authentificated or not.
            void resetAuthStatus(void) noexcept
            { this->AUTH_STATUS = false; }

            // This function is used to reset the website session ID.
            void resetSessionID(void) noexcept
            { this->SESSION_ID = -1; }

            // This function is used to empty the 'fileInQueue' string object.
            void clearFileInQueue(void) noexcept
            { this->fileInQueue.clear(); }

            // This functions asigns to the 'fileInQueue' variable the name of the file uploaded to the Server
            void addFileInQueue(const std::string &file_name) noexcept
            { this->fileInQueue = file_name; }

            // This functions adds 'userCredential' objects to the 'uc' vector. It uses the 'push_back' function.
            void addToUserCredentials(const user_credentials &__uc) noexcept
            { this->uc.push_back(__uc); }

            // This functions adds 'user_files' objects to the 'uf' vector. It uses the 'push_back' function.
            void addToUserFiles(const user_files &__uf) noexcept
            { this->uf.push_back(__uf); }

            /**
             *
             * @brief This function handles the '/userlogin' route, sending an appropriate HTTP response to the client.
             *
             * @param buffer Contains the request data.
             * @param acceptedSocketFileDescriptor The file descriptor for the accepted socket connection used when seding the HTTP response.
             *
             * @return Returns 0 on success, 1 for errors.
             *
             */
            int loginRoute(char *buffer, int acceptedSocketFileDescriptor);

            /**
             *
             * @brief This function handles the '/addFile' route, sending an appropriate HTTP response to the client.
             *
             * @param buffer Contains the file data. This is used to retrive the metadata of the request such as the file name and file extension.
             * @param byteBuffer Contains the file data. This is used to write the data into an actual file on the local machine.
             * @param acceptedSocketFileDescriptor The file descriptor for the accepted socket connection used when seding the HTTP response.
             * @param bytesReceived The size of the current buffer / byteBuffer
             *
             * @return Returns 0 on success, 1 for errors.
             *
             */
            int addFilesRoute(const char *buffer, const uint8_t *byteBuffer, int acceptedSocketFileDescriptor, ssize_t bytesReceived);

            int format_file(const std::string file_name);

            /**
             *
             * @brief This function handles the '/change_password' route, sending an appropriate HTTP response to the client.
             *
             * @param buffer Contains the request data.
             * @param acceptedSocketFileDescriptor The file descriptor for the accepted socket connection used when seding the HTTP response.
             *
             * @return Returns 0 on success, 1 for errors.
             *
             */
            int changePasswordRoute(char *buffer, int acceptedSocketFileDescriptor);

            /**
             *
             * @brief This function handles the '/create_account' route, sending an appropriate HTTP response to the client.
             *
             * @param buffer Contains the request data.
             * @param acceptedSocketFileDescriptor The file descriptor for the accepted socket connection used when seding the HTTP response.
             *
             * @return Returns 0 on success, 1 for errors.
             *
             */
            int createAccountRoute(char *buffer, int acceptedSocketFileDescriptor);

            /**
             *
             * @brief This function handles the '/delete_files' route, sending an appropriate HTTP response to the client.
             *        It deletes the files from the Server. (automatically the files on the local machine will be deleted as well)
             *
             * @param buffer Contains the request data.
             * @param acceptedSocketFileDescriptor The file descriptor for the accepted socket connection used when seding the HTTP response.
             *
             * @return Returns 0 on success, 1 for errors.
             *
             */
            int deleteFileRoute(char *buffer, int acceptedSocketFileDescriptor);

            /**
             * @brief This function is used to dynamically build 'index.html' during runtime.
             *        When a new file is uploaded to the Server the page is rebuilt, generating a number of <tr> elements equal to the number of files sent.
             */
            void buildIndexHTML(void);

            // This function retrieves the "user" table from the database
            void SQLfetchUserTable(void);

            // This function retrieves the "file" table from the database
            void SQLfetchFileTable(void);

            /**
             * @brief This function adds the files uploaded to the Server.
             * @return Returns 0 on success, 1 for errors.
             */
            int addToFileTable(const char *fileName, const double fileSize);

        private:
            /**
             * @brief This function validates user credentials.
             * @return Returns true on success and false if the user credentials are not valid.
             */
            bool validateCredentials(const char username[], const char password[]);

            /**
             * @brief This function searches for a username if it has already been used.
             * @return Returns true on success and false if the username is not found.
             */
            bool findUsername(const char username[]);

            bool AUTH_STATUS; // This variable is used to check is the user is authentificated or not
            int SESSION_ID;   // This variable is used to load the correct user data. It is asigned the ID of the user who logs into their account.

            // Vector that stores all user credentials objects (This is not the same as the database credentials)
            std::vector<user_credentials> uc;

            // Vector that stores all user files objects
            std::vector<user_files> uf;

            // This variable stores the name of the last file that was uploaded to the Server. It is used to insert the newly added data into the database.
            std::string fileInQueue;

            MySQL_Handle *m_mysql_handle;
        };
    };
};