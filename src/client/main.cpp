#include "../socket/socketUtils.hpp"
#include "clientUtils.hpp"

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <unistd.h>

int main()
{
    net::SocketUtils *socket = new net::SocketUtils;
    net::client *client = new net::client;

    int socketFD = socket->createSocket();
    
    if (socketFD == -1) {
        std::cerr << "Socket creation failed\n";
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in *address = socket->IPv4Address(localHostAddress, PORT);

    int res = socket->connectToServer(socketFD, address);

    if (res == -1) {
        std::cerr << "Connection failed\n";
        free(address);
        delete socket;
        return EXIT_FAILURE;
    }

    std::cout << "Connection established!\n";
    
    char *buffer = nullptr;
    size_t bufferSize = 0;

    char *clientName = nullptr;
    size_t clientNameSize = 0;
    std::cout << "Name: ";
    ssize_t clientNameCount = getline(&clientName, &clientNameSize, stdin);

    if (clientName[clientNameCount - 1] == '\n')
        clientName[clientNameCount - 1] = '\0';

    clientName[clientNameCount - 1] = ':';
    clientNameCount++;
    clientName[clientNameCount - 1] = '\0';

    client->__INIT_MESSAGE_LISTENER_THREAD__(socketFD);
    
    while (true)
    {
        ssize_t charCount = getline(&buffer, &bufferSize, stdin);

        strcat(clientName, buffer);

        if (charCount > 0)
        {
            if (strcasecmp(buffer, "exit") == 0)
                break;

            ssize_t amountSent = send(socketFD, clientName, strlen(clientName), 0);
        }
    }

    close(socketFD);

    free(address);
    delete socket;

    return EXIT_SUCCESS;
}
