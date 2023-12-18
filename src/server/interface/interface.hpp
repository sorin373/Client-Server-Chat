#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#include <vector>

extern volatile int userCredentialsCount;
extern char tableName[];

namespace net
{
    namespace interface 
    {
        void writeHTMLhead(void);
        void readFiles(void);

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
            std::vector<user::userCredentials> uc;  // log in user credentials
            
        public:
            user() = default;

            std::vector<class userCredentials> getUserCredentials(void) const noexcept;
            bool routeHandler(char *request, int acceptedSocketFileDescriptor);
            void addToUserCredentials(const userCredentials &__uc) noexcept;
            void resizeUserCredentialsVector(void) noexcept;
            bool validateCredentials(char *username, char *password) const;

            ~user() = default;
        };
    }
}

#endif