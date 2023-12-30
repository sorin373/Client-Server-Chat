/**
 *
 * @file          main.cpp
 *
 * @copyright     MIT License
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
 * Comaptibility: The application was built and tested on Ubuntu 22.04. In the GitHub repository the latest DEB install package is
 *                available for download.
 *
 * @author        Sorin Tudose
 *
 * @brief         This is a simple C++ HTTP server implementation using the Linux Socket API. The main feature is the ability to upload and download
 *                files using HTTP requests. Consequently, the data fed into the server is stored locally on my personal computer in a MySQL database
 *                using the MySQL cpp connector library.
 *
 *                (https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-getting-started-examples.html)
 *
 * Github:        https://github.com/sorin373/HTTP-Server
 *
 * Test it:       In order to successfully run the server on your local machine you first need to setup a database. I uploaded the table formats int GitHub repository
 *                as CSV files which can be used to get the app running quickly. Moreover, keep in mind that the executable must be run with SUDO as it needs
 *                extra permissions to write / read the application files.
 *
 *                Thank you for checking out my project! :D
 *
 */

#include "httpServer.hpp"

#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>

using namespace net;

int main(int argc, char *argv[])
{
    int port = 0;

    port = getMainArguments(argc, argv);

    if (port == -1)
        return EXIT_FAILURE;

    SocketUtils serverSocket;

    int serverSocketFD = serverSocket.createSocket();

    if (serverSocketFD == -1)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Socket creation failed.\n";

        return EXIT_FAILURE;
    }

    __server = new server<char>();

    if (__server->__database_init__() == EXIT_FAILURE)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Database initialization failed.\n";

        shutdown(serverSocketFD, SHUT_RDWR);
        delete __server;
        serverSocket.closeSocket(serverSocketFD);

        return EXIT_FAILURE;
    }

    char *machineIPv4Address = serverSocket.getMachineIPv4Address();

    if (machineIPv4Address == nullptr)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Null pointer encountered for machine's IPv4 address.\n";

        serverSocket.closeSocket(serverSocketFD);
        shutdown(serverSocketFD, SHUT_RDWR);
        delete __server;

        return EXIT_FAILURE;
    }

    struct sockaddr_in *serverAddress = serverSocket.IPv4Address(machineIPv4Address, port);

    if (__server->bindServer(serverSocketFD, serverAddress) != 0)
    {
        std::cerr << "\n"
                  << std::setw(5) << " "
                  << "--> Server encountered an error during the binding process.\n"
                  << std::setw(5) << " "
                  << "--> ";

        perror("Bind");

        std::cout << ".\n";

        serverSocket.closeSocket(serverSocketFD);
        shutdown(serverSocketFD, SHUT_RDWR);
        free(serverAddress);
        delete __server;
        delete[] machineIPv4Address;

        return EXIT_FAILURE;
    }

    system("clear");

    std::cout << "\n\n"
              << std::setw(5) << " "
              << "+-------------+\n"
              << std::setw(5) << " "
              << "| HTTP-SERVER |\n"
              << std::setw(5) << " "
              << "+-------------+\n";

    underline(75);

    std::cout << std::setw(5) << " "
              << "--> Database authentication successful!\n"
              << std::setw(5) << " "
              << "--> Server socket bound successfully!\n"
              << std::setw(5) << " "
              << "--> Access the server through your web browser via: \e[1m" << machineIPv4Address << ":" << port << "\e[0m\n"
              << std::setw(5) << " "
              << "    In order to shutdown the server type 'exit' in the console!\n";

    if (listen(serverSocketFD, 10) == -1)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Failed to initiate listening on the server socket.\n";

        serverSocket.closeSocket(serverSocketFD);
        shutdown(serverSocketFD, SHUT_RDWR);
        free(serverAddress);
        delete __server;

        return EXIT_FAILURE;
    }

    __server->__SERVER_INIT__(serverSocketFD);

    serverSocket.closeSocket(serverSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);
    free(serverAddress);
    delete __server;
    delete[] machineIPv4Address;

    return EXIT_SUCCESS;
}