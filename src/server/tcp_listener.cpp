#include "tcp_listener.hpp"

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096

using namespace net;

void net::ZeroMemory(void *ptr, unsigned int size)
{
    BYTE *cptr = static_cast<BYTE*>(ptr);

    for (unsigned int i = 0; i < size; i++)
        cptr[i] = 0;
}

int tcp_listener::init()
{
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (m_socket == INVALID_SOCKET)
    {
        std::cerr << "--> socket() falied!\n";
        return -1;
    }

    struct sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(m_port);
    inet_pton(AF_INET, m_ip_address, &hint.sin_addr);

    if (bind(m_socket, (sockaddr*)&hint, sizeof(sockaddr_in)) == INVALID_SOCKET)
    {
        std::cerr << "--> bind() failed!\n";
        return -1;
    }

    if (listen(m_socket, SOMAXCONN) == INVALID_SOCKET)
    {
        std::cerr << "--> listen() failed!\n";
        return -1;
    }

    FD_ZERO(&m_master);

    FD_SET(m_socket, &m_master);

    return EXIT_SUCCESS;
}

int tcp_listener::run()
{
    bool running = true;

    while (running)
    {
        fd_set copy = m_master;

        int socket_count = select(FD_SETSIZE, &copy, nullptr, nullptr, nullptr);
        
        for (SOCKET socket = 0; socket < FD_SETSIZE; ++socket) 
        {
            if (FD_ISSET(socket, &copy)) 
            {
                if (socket == m_socket) 
                {
                    SOCKET client = accept(m_socket, nullptr, nullptr);

                    if (client == INVALID_SOCKET)
                        std::cerr << "--> accept() failed\n";
                    else 
                    {
                        FD_SET(client, &m_master);
                        on_client_connected(client);
                    }
                } 
                else 
                {
                    char buffer[BUFFER_SIZE];
                    net::ZeroMemory(buffer, BUFFER_SIZE);

                    int bytes_in = recv(socket, buffer, BUFFER_SIZE, 0);

                    if (bytes_in <= 0) 
                    {
                        on_client_disconnected(socket);
                        FD_CLR(socket, &m_master);
                        close(socket);
                    } 
                    else 
                    {
                        on_message_received(socket, buffer, bytes_in);
                    }
                }
            }
        }
    }

    FD_CLR(m_socket, &m_master);
    close(m_socket);

    for (int fd = 0; fd < FD_SETSIZE; fd++)
        if (FD_ISSET(fd, &m_master))
        {
            FD_CLR(fd, &m_master);
            close(fd);
        }

    return EXIT_SUCCESS;
}

void tcp_listener::send_to_client(SOCKET client_socket, const char *msg, unsigned int size)
{
    if (send(client_socket, msg, size, 0) == -1)
        std::cerr << "==TCP== send() failed: " << msg << "\n";
}

void tcp_listener::on_client_connected(SOCKET client_socket) { }

void tcp_listener::on_client_disconnected(SOCKET client_socket) { }

void tcp_listener::on_message_received(SOCKET client_socket, char *msg, unsigned int size) { }