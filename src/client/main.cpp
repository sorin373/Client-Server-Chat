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

    char *clientName = client->getClientName();

    client->__INIT_MESSAGE_LISTENER_THREAD__(socketFD);
    
    while (true)
    {
        ssize_t charCount = getline(&buffer, &bufferSize, stdin);

        strcat(clientName, buffer);

        if (charCount > 0)
        {
            if (strcasecmp(buffer, "exit\n") == 0)
                break;

            ssize_t amountSent = client->sendData<char>(socketFD, clientName, strlen(clientName) + 1);
        }
    }

    close(socketFD);

    free(address);
    delete socket;

    return EXIT_SUCCESS;
}
