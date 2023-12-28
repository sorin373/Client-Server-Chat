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

    __server = new server(serverSocketFD);

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
        std::cerr << std::setw(5) << " "
                  << "--> Server encountered an error during the binding process.\n";
        perror("bind");

        serverSocket.closeSocket(serverSocketFD);
        shutdown(serverSocketFD, SHUT_RDWR);
        free(serverAddress);
        delete __server;
        delete[] machineIPv4Address;

        return EXIT_FAILURE;
    }

    system("clear");

    std::cout << "\n\n"
              << std::setw(30) << " "
              << "+-------------+\n"
              << std::setw(30) << " "
              << "| HTTP-SERVER |\n"
              << std::setw(30) << " "
              << "+-------------+\n";

    underline(75);

    std::cout << std::setw(5) << " "
              << "--> Server socket bound successfully!\n"
              << std::setw(5) << " "
              << "--> Access the server through your web browser via: \e[1m" << machineIPv4Address << ":" << port << "\e[0m\n";

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

    __server->__SERVER_INIT__<char>(serverSocketFD);

    serverSocket.closeSocket(serverSocketFD);
    shutdown(serverSocketFD, SHUT_RDWR);
    free(serverAddress);
    delete __server;
    delete[] machineIPv4Address;

    return EXIT_SUCCESS;
}