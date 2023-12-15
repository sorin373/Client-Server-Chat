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
                userCredentials() = default;

                bool validateCredentials(char *username, char *password);
                void constructObject(char *username, char *password, const int id);

                ~userCredentials() = default;
            };

        private:
            std::vector<user::userCredentials> uc;
            
        public:
            user() = default;

            std::vector<class user::userCredentials> getUserCredentials(void) const noexcept;
            static bool routeHandler(char *request, int acceptedSocketFileDescriptor);
            void addToUserCredentials(char *username, char *password, const int id) noexcept;
            void resizeUserCredentialsVector(void) noexcept;

            ~user() = default;
        };
    }
}

#endif