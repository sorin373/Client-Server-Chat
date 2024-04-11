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

#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#include <vector>
#include <string>
#include <stdlib.h>

#define NET_USERNAME_LENGHT 32
#define NET_PASSWORD_LENGHT 64

namespace net
{
    namespace interface
    {
        // Class describing a 'user' and it is part of the Server's interface namespace.
        class User
        {
        public:
            // Nested class describing the user credentials object.
            class userCredentials
            {
            private:
                char *username, *password;
                int id;

            public:
                userCredentials(const char *username, const char *password, const int id);

                char *getUsername(void) const noexcept;
                char *getPassword(void) const noexcept;
                int getId(void) const noexcept;

                ~userCredentials() = default;
            };

        public:
            // Nested class describing the user files object.
            class userFiles
            {
            private:
                char *fileName, *date;
                int userID, fileID, fileSize;

            public:
                userFiles(const char *fileName, const int userID, const int fileID, const int fileSize, const char *date);

                char *getFileName(void) const noexcept;
                int getUserID(void) const noexcept;
                int getFileID(void) const noexcept;
                int getFileSize(void) const noexcept;
                char *getDate(void) const noexcept;

                ~userFiles() = default;
            };

        private:
            bool AUTH_STATUS; // This variable is used to check is the user is authentificated or not
            int SESSION_ID;   // This variable is used to load the correct user data. It is asigned the ID of the user who logs into their account.

            // Vector that stores all user credentials objects (This is not the same as the database credentials)
            std::vector<class userCredentials> uc;

            // Vector that stores all user files objects
            std::vector<class userFiles> uf;

            // This variable stores the name of the last file that was uploaded to the Server. It is used to insert the newly added data into the database.
            std::string fileInQueue;

            /**
             * @brief This function validates user credentials.
             * @return Returns true on success and false if the user credentials are not valid.
             */
            bool validateCredentials(const char username[], const char password[]);
            bool findUsername(const char username[]);

        public:
            User();

            // This function retrieves a vector where the user credentials are stored
            std::vector<class userCredentials> getUserCredentials(void) const noexcept;

            // This function retrieves a vector where the user files are stored
            std::vector<class userFiles> getUserFiles(void) const noexcept;

            // This function retrieves a string that holdes the name of the file that needs to be inserted in the database
            std::string getFileInQueue(void) const noexcept;

            // This function retrieves the session ID
            int getSessionID(void) const noexcept;

            /**
             * @brief This function retrieves the authentification status.
             * @return Returns true if the user is signed in, false otherwise.
             */
            bool getAuthStatus(void) const noexcept;

            // This function is used to reset the variable that is used to check if the user is authentificated or not.
            void resetAuthStatus(void) noexcept;

            // This function is used to reset the website session ID.
            void resetSessionID(void) noexcept;

            // This function is used to empty the 'uc' vector.
            void clearUserCredentials(void) noexcept;

            // This function is used to empty the 'uf' vector.
            void clearUserFiles(void) noexcept;

            // This function is used to empty the 'fileInQueue' string object.
            void clearFileInQueue(void) noexcept;

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

            // This functions adds 'userCredential' objects to the 'uc' vector. It uses the 'push_back' function.
            void addToUserCredentials(const userCredentials t_uc) noexcept;

            // This functions adds 'userFiles' objects to the 'uf' vector. It uses the 'push_back' function.
            void addToUserFiles(const userFiles t_uf) noexcept;

            // This functions asigns to the 'fileInQueue' variable the name of the file uploaded to the Server
            void addFileInQueue(const std::string fileName) noexcept;

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
            int addToFileTable(const char *fileName, const int fileSize);

            ~User();
        };
    };
};

#endif // __INTERFACE_HPP__