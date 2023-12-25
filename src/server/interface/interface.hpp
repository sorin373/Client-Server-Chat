#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#define INDEX_HTML "interface/index.html"

#include <vector>
#include <string>
#include <stdlib.h>

namespace net
{
    namespace interface 
    {
        class user
        {
        public:
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
            class userFiles
            {
            private:
                char *fileName;
                int id, fileSize, noDownloads;

            public:
                userFiles(const char *fileName, const int id, const int fileSize, const int noDownloads);

                char *getFileName(void) const noexcept;
                int getId(void) const noexcept;
                int getFileSize(void) const noexcept;
                int getNoDownloads(void) const noexcept;

                ~userFiles() = default;
            };

        private:
            int SESSION_ID;
            std::vector<class userCredentials> uc; // log in user credentials
            std::vector<class userFiles>       uf; // user files by SESSION ID
            std::string fileInQueue;               // remaining file names to be pushed into the database
            
        public:
            user();

            std::vector<class userCredentials> getUserCredentials(void) const noexcept;
            std::vector<class userFiles> getUserFiles(void) const noexcept;
            std::string getFileInQueue(void) const noexcept;
            int getSessionID(void) const noexcept;

            int getUserCredentialsSize(void) const noexcept;

            void clearUserCredentials(void) noexcept;
            void clearUserFiles(void) noexcept;
            void clearFileInQueue(void) noexcept;

            int loginRoute(char *request, int acceptedSocketFileDescriptor);
            int addFilesRoute(const char *buffer, const uint8_t *byteBuffer, int acceptedSocketFileDescriptor, ssize_t __bytesReceived);

            void addToUserCredentials(const userCredentials __uc) noexcept;
            void addToUserFiles(const userFiles __uf) noexcept;
            void addFileInQueue(const std::string fileName) noexcept;

            bool validateCredentials(char *username, char *password);

            void buildIndexHTML(void);

            ~user() = default;
        };
    }
}

#endif