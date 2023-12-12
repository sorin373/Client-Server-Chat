#ifndef __INTERFACE_HPP__
#define __INTERFACE_HPP__

#include "../serverUtils.hpp"

namespace net
{
    namespace interface 
    {
        class user
        {
        private:
            struct userCredentials
            {
                int id;
                char *username, *password;

                userCredentials(char *username, char *password, const int id);
                ~userCredentials() = default;
            };

        static userCredentials *uc;
            
        public:
            user();

            static void fetchCredentials(char *username, char *password, const int id);
            static userCredentials *getUserCredentials(void) noexcept;
            static void cleanup(void);

            ~user() = default;
        };

        void writeHTMLhead(void);
        void readFiles(void);

        bool routeHandler(char *request);
    }
}

#endif