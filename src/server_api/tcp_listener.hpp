#pragma once

#include "sconfig.hpp"

#include <sys/select.h>

namespace net
{
    inline void ZeroMemory(void *ptr, const unsigned int size)
    { 
        if (ptr == nullptr)
            return;

        BYTE *__ptr = static_cast<BYTE*>(ptr);

        for (unsigned int i = 0; i < size; ++i)
            *(__ptr + i) = 0;
    }

    class tcp_listener
    {
    public:
        tcp_listener() noexcept { }

        tcp_listener(const char *ip_address, int port) 
            : m_ip_address(ip_address), m_port(port) { }

        int tcp_easy_init();

        int run();

    protected:
        virtual void on_client_connected(SOCKET client_socket);

        virtual void on_client_disconnected(SOCKET client_socket);

        virtual void on_message_received(SOCKET client_socket, char *msg, unsigned int size);

        void send_to_client(SOCKET client_socket, const char *msg, unsigned int size);

    private:
        const char  *m_ip_address;
        int          m_port;
        SOCKET       m_socket;
        fd_set       m_master;
    };
}