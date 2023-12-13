#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#include "../serverUtils.hpp"

namespace net
{
    namespace interface 
    {
        void writeHTMLhead(void);
        void readFiles(void);

        class user
        {
            struct userCredentials
            {
            private:
                int id;
                char *username, *password;
            public:
                userCredentials(char *username, char *password, const int id);
                ~userCredentials() = default;
            };

        private:
            userCredentials *uc;
            
        public:
            user();

            void fetchCredentials(char *username, char *password, const int id);
            userCredentials *getUserCredentials(void) noexcept;
            static bool routeHandler(char *request, int acceptedSocketFileDescriptor);
            void cleanup(void);

            ~user() = default;
        };
    }
}

#endif