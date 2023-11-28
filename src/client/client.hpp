#pragma once
#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

namespace net
{
    namespace messaging
    {
        class message
        {
        public:
            message();

            void sendMessage(void);

            ~message();
        };
    };
};

#endif