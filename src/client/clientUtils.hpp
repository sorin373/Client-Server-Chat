#pragma once
#ifndef __CLIENT_UTILS_HPP__
#define __CLIENT_UTILS_HPP__

namespace net
{
    class client 
    {
    private:
        void __MESSAGE_LISTENER_THREAD__(int clientSocketFileDescriptor);

    public:
        client() = default;

        void __INIT_MESSAGE_LISTENER_THREAD__(int clientSocketFileDescriptor);
        void sendData(void);

        ~client() = default;
    };
};

#endif