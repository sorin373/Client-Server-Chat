/**
 *
 *  @file         socketUtils.hpp
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
 *  @brief        This C++ header file offers utility functions for socket operations.
 *
 */

#ifndef __SOCKET_UTILS_HPP__
#define __SOCKET_UTILS_HPP__

#include <netinet/in.h>

namespace net
{
    constexpr char LOCAL_HOST_ADDRESS[] = "127.0.0.1";
    constexpr int DEFAULT_PORT = 8080;

    class SocketUtils
    {
    public:
        SocketUtils() = default;

        /**
         * @brief This function retrieves the machine IP address using the ifconfig Linux command
         * @return Returns nullptr for errors.
         */
        static char *getMachineIPv4Address(void);

        /**
         * @brief This function creates a new socket of type SOCK_STREAM in domain AF_INET, using protocol 1 (Internet Control Message Protocol).
         * @return Returns a file descriptor for the new socket, or -1 for errors.
         */
        int createSocket(int addressFamily, int socketType, int protocol);

        /**
         * @brief This function opens a connection on the socket file descriptor.
         * @return Returns 0 on success, -1 for errors.
         */
        int connectToServer(int socketFileDescriptor, struct sockaddr_in *address);

        /**
         *  @brief This function configures an IPv4 address structure based on the provided IP address and port number. Returns 0 on success, -1 for errors.
         *  @attention Default address: 127.0.0.1
         *  @attention Default port:    8080
         *  @return machine address, nullptr for errors
         */
        struct sockaddr_in *IPv4Address(const char *ipAddress = LOCAL_HOST_ADDRESS, int port = DEFAULT_PORT);

        // This function closes the file descriptor of a socket
        void closeSocket(int socketFileDescriptor);

        ~SocketUtils() = default;
    };
};

#endif // __SOCKET_UTILS_HPP__