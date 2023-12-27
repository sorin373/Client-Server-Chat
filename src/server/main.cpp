#include "serverUtils.hpp"
#include "declarations.hpp"

#include <iostream>
#include <fstream>

using namespace net;

int main(int argc, char *argv[])
{
    std::ofstream index;
    index.open("interface/index.html", std::ofstream::out | std::ofstream::trunc);
    index.close();

    int port = 0;

    if (argc < 2)
        port = DEFAULT_PORT;
    else
        port = atoi(argv[1]);

    SocketUtils serverSocket;

    int serverSocketFD = serverSocket.createSocket();

    if (serverSocketFD == -1)
    {
        std::cerr << "Failed to create socket!\n";

        return EXIT_FAILURE;
    }

    __server = new server(serverSocketFD);

    if (__server->__database_init__() == EXIT_FAILURE)
    {
        shutdown(serverSocketFD, SHUT_RDWR);
        delete __server;

        return EXIT_FAILURE;
    }

    char *machineIPv4Address = serverSocket.getMachineIPv4Address();

    if (machineIPv4Address == nullptr)
    {
        shutdown(serverSocketFD, SHUT_RDWR);
        delete __server;

        return EXIT_FAILURE;
    }

    std::cout << machineIPv4Address << ":" << port << "\n";

    struct sockaddr_in *serverAddress = serverSocket.IPv4Address(machineIPv4Address, port);

    if (__server->bindServer(serverSocketFD, serverAddress) != 0)
    {
        std::cerr << "Error binding the server!\n";
        perror("bind");

        shutdown(serverSocketFD, SHUT_RDWR);
        free(serverAddress);
        delete __server;
        delete[] machineIPv4Address;

        return EXIT_FAILURE;
    }

    std::cout << "Server socket bound successfully!\n";

    if (listen(serverSocketFD, 10) == -1)
    {
        shutdown(serverSocketFD, SHUT_RDWR);
        free(serverAddress);
        delete __server;

        return EXIT_FAILURE;
    }

    __server->__MASTER_THREAD__(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);
    free(serverAddress);
    delete __server;
    delete[] machineIPv4Address;

    return EXIT_SUCCESS;

    return 0;
}