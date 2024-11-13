#include "tcp_listener.hpp"

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096

namespace net
{
    int tcp_listener::tcp_easy_init()
    {
        this->m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (this->m_socket == INVALID_SOCKET)
        {
            std::cerr << "  --> socket() falied!\n";
            return -1;
        }

        struct sockaddr_in hint;
        
        hint.sin_family = AF_INET;
        hint.sin_port = htons(m_port);
        inet_pton(AF_INET, this->m_ip_address, &hint.sin_addr);

        if (bind(this->m_socket, (sockaddr*)&hint, sizeof(sockaddr_in)) == INVALID_SOCKET)
        {
            std::cerr << "  --> bind() failed!\n";
            return -1;
        }

        if (listen(this->m_socket, SOMAXCONN) == INVALID_SOCKET)
        {
            std::cerr << "  --> listen() failed!\n";
            return -1;
        }

        FD_ZERO(&this->m_master);

        FD_SET(this->m_socket, &this->m_master);

        return 0;
    }

    int tcp_listener::run()
    {
        std::cout << "  --> The server is live!\n";

        bool running = true;

        while (running)
        {
            fd_set copy = this->m_master;

            int socket_count = select(FD_SETSIZE, &copy, nullptr, nullptr, nullptr);
            
            for (SOCKET socket = 0; socket < FD_SETSIZE; ++socket) 
            {
                if (FD_ISSET(socket, &copy)) 
                {
                    if (socket == this->m_socket) 
                    {
                        SOCKET client = accept(m_socket, nullptr, nullptr);
    
                        if (client == INVALID_SOCKET)
                            std::cerr << "--> accept() failed\n";
                        else 
                        {
                            FD_SET(client, &this->m_master);
                            this->on_client_connected(client);
                        }
                    } 
                    else 
                    {
                        char buffer[BUFFER_SIZE];
                        ZeroMemory(buffer, BUFFER_SIZE);

                        int bytes_in = recv(socket, buffer, BUFFER_SIZE, 0);

                        if (bytes_in <= 0) 
                        {
                            this->on_client_disconnected(socket);
                            FD_CLR(socket, &this->m_master);
                            close(socket);
                        } 
                        else 
                        {
                            this->on_message_received(socket, buffer, bytes_in);
                        }
                    }
                }
            }
        }

        FD_CLR(this->m_socket, &this->m_master);
        close(this->m_socket);

        for (int fd = 0; fd < FD_SETSIZE; ++fd)
            if (FD_ISSET(fd, &this->m_master))
            {
                FD_CLR(fd, &this->m_master);
                close(fd);
            }

        return 0;
    }

    void tcp_listener::send_to_client(SOCKET client_socket, const char *msg, unsigned int size)
    {
        if (send(client_socket, msg, size, 0) == -1)
            std::cerr << "==TCP== send() failed: " << msg << "\n";
    }

    void tcp_listener::on_client_connected(SOCKET client_socket) { }

    void tcp_listener::on_client_disconnected(SOCKET client_socket) { }

    void tcp_listener::on_message_received(SOCKET client_socket, char *msg, unsigned int size) { }
}