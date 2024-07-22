#ifndef __TCP_LISTENER_H__
#define __TCP_LISTENER_H__

#include <sys/select.h>

#define INVALID_SOCKET -1

typedef int           SOCKET;
typedef unsigned char BYTE;

namespace net
{
    void ZeroMemory(void *ptr, unsigned int size);

    class tcp_listener
    {
    public:
        tcp_listener() noexcept { }

        tcp_listener(const char *ip_address, int port) : m_ip_address(ip_address), m_port(port) { }

        int init();

        int run();

    protected:
        virtual void on_client_connected(SOCKET client_socket);

        virtual void on_client_disconnected(SOCKET client_socket);

        virtual void on_message_received(SOCKET client_socket, char *msg, unsigned int size);

        void send_to_client(SOCKET client_socket, const char *msg, unsigned int size);

    private:
        const char *m_ip_address;
        int         m_port;
        SOCKET      m_socket;
        fd_set      m_master;
    };
}

#endif