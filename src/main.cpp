#include "httpServer.hpp"

#include <iostream>
#include <fstream>

using namespace net;

int main(int argc, char *argv[])
{
    int port = 0;

    if (argc > 2)
    {
        std::cerr << "Invalid number of arguments provided.\n"
                  << "Usage: ./httpServer [port]\n"
                  << "If no port is provided, the default port will be used.\n";

        return EXIT_FAILURE;
    }

    if (argc == 1)
        port = DEFAULT_PORT;
    else if (argc == 2)
        if (isNumeric(argv[1]))
            port = atoi(argv[1]);

    std::cout << port << std::endl;
            
    if (port == 0)
    {
        std::cerr << "Invalid port number provided. Please use a valid port number (e.g., 5000).\n";
        std::cerr << "Usage: ./httpServer [port]\n";
        std::cerr << "If no port is provided, the default port will be used.\n";

        return EXIT_FAILURE;
    }
        
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

    __server->__MASTER_THREAD__<char>(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);
    free(serverAddress);
    delete __server;
    delete[] machineIPv4Address;

    return EXIT_SUCCESS;
}