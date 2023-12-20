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
        void buildIndexHTML(void);

        class user
        {
        public:
            class userCredentials
            {
            private:
                char *username, *password;
                int id;
            public:
                userCredentials(const char *__username = nullptr, const char *__password = nullptr, const int id = 0);

                char *getUsername(void) const noexcept;
                char *getPassword(void) const noexcept;
                int getId(void) const noexcept;

                ~userCredentials();
            };

        private:
            long long unsigned int SESSION_ID;
            std::vector<user::userCredentials> uc;  // log in user credentials
            
        public:
            user() = default;

            std::vector<class userCredentials> getUserCredentials(void) const noexcept;
            long long unsigned int getSessionID(void) const noexcept;
            int routeHandler(char *request, int acceptedSocketFileDescriptor);
            void addToUserCredentials(const userCredentials &__uc) noexcept;
            void resizeUserCredentialsVector(void) noexcept;
            bool validateCredentials(char *username, char *password) const;

            ~user() = default;
        };
    }
}

#endif