/**
 *
 *  @file         web_server.hpp
 *
 *  @copyright    MIT License
 *
 *                Copyright (c) 2023 Sorin Tudose
 *
 *                Permission is hereby granted, free of charge, to any person obtaining a copy
 *                of this software and associated documentation files (the "Software"), to deal
 *                in the Software without restriction, including without limitation the rights
 *                to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *                copies of the Software, and to permit persons to whom the Software is
 *                furnished to do so, subject to the following conditions:
 *
 *                The above copyright notice and this permission notice shall be included in all
 *                copies or substantial portions of the Software.
 *
 *                THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *                IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *                FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *                AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *                LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *                OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *                SOFTWARE.
 *
 *  @brief        This C++ header file declares the 'web_server' class which contains core functionalites of an HTTP Server.
 *
 */

#pragma once

#include "tcp_listener.hpp"
#include "interface/interface.hpp"

namespace net
{
    // Class implementing core functionalities of an HTTP Server.
    class web_server : public tcp_listener
    {
    public:
        web_server(const char *ip_address, int port) 
            : tcp_listener(ip_address, port), m_user(),
                 m_request_type(nullptr), m_current_route(nullptr), m_change_route(true) { }

        /**
         * @brief Master initialization function for the server functionalities.
         * 
         * @param argc Number of command-line arguments.
         * @param argv Array of command-line arguments.
         * 
         * @return Returns 0 on success, 1 for errors.
         */
        int ws_init(int argc, char* argv[]);

    protected:
        virtual void on_client_connected(SOCKET client_socket) override
        { std::cout << "--> Client socket connected: " << client_socket << "\n"; }

        /**
         * @brief This function performs post receive operations such as:
         *        file formatting, adding the file metadata to the database and clearing the file from the queue.
         * @param client_socket The file descriptor for the accepted socket connection used when seding the HTTP response.
         */
        virtual void on_client_disconnected(SOCKET client_socket) override;

        virtual void on_message_received(SOCKET client_socket, char *msg, unsigned int size) override;

    private:
        /**
         * @brief This function decides whether the HTTP request is a POST or GET request.
         *
         * @param buffer Contains the request data.
         * @param client_socket_FD The file descriptor for the accepted socket connection used when seding the HTTP response.
         * @param bytes_in The size of the current buffer
         *
         * @return Returns 0 on success, 1 for errors.
         */
        int request_handler(char *buffer, SOCKET client_socket_FD, unsigned int bytes_in);

        /**
         * @brief This function handles HTTP GET requests.
         *
         * @param buffer Contains the request data.
         * @param client_socket_FD The file descriptor for the accepted socket connection used when seding the HTTP response.
         *
         * @return Returns 0 on success, 1 for errors.
         */
        int GET_request_handler(char *buffer, SOCKET client_socket_FD);

        /**
         * @brief This function handles HTTP POST requests.
         *
         * @param buffer Contains the request data.
         * @param client_socket_FD The file descriptor for the accepted socket connection used when seding the HTTP response.
         * @param bytes_in The size of the current buffer
         *
         * @return Returns 0 on success, 1 for errors.
         */
        int POST_request_handler(char *buffer, SOCKET client_socket_FD, unsigned int bytes_in);

        void free_request_type()
        {
            if (this->m_request_type != nullptr)
            {
                free(this->m_request_type);
                this->m_request_type = nullptr;
            }
        }

        void free_current_route()
        {
            if (this->m_current_route != nullptr)
            {
                free(this->m_current_route);
                this->m_current_route = nullptr;
            }
        }

        interface::user  m_user;
        char            *m_request_type;
        char            *m_current_route;
        bool             m_change_route;
    };
}