#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#define STORAGE_FILE_NAMES "interface/storage/fileNames.txt"
#define INDEX_HTML         "interface/index.html"

#include <vector>

extern volatile int userCredentialsCount;
extern char tableName[];

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
                userCredentials(const char *username = nullptr, const char *password = nullptr, const int id = 0);

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
                long long unsigned int getId(void) const noexcept;
                int getFileSize(void) const noexcept;
                int getNoDownloads(void) const noexcept;

                ~userFiles() = default;
            };

        private:
            int SESSION_ID;
            std::vector<user::userCredentials> uc;  // log in user credentials
            std::vector<user::userFiles>       uf;  // user files by SESSION ID
            
        public:
            user();

            std::vector<class userCredentials> getUserCredentials(void) const noexcept;
            std::vector<class userFiles> getUserFiles(void) const noexcept;
            int getSessionID(void) const noexcept;

            void clearUserCredentials(void) noexcept;
            void clearUserFiles(void) noexcept;

            int routeHandler(char *request, int acceptedSocketFileDescriptor);

            void addToUserCredentials(const userCredentials __uc) noexcept;
            void addToUserFiles(const userFiles __uf) noexcept;

            void resizeUserCredentialsVector(void) noexcept;
            bool validateCredentials(char *username, char *password);

            void buildIndexHTML(void);

            ~user() = default;
        };
    }
}

#endif