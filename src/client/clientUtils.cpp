#include "clientUtils.hpp"
#include "../socket/socketUtils.hpp"

#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>

void net::client::__MESSAGE_LISTENER_THREAD__(int clientSocketFileDescriptor)
{
    char buffer[1024];

    while (true)
    {
        ssize_t bytesReceived = recv(clientSocketFileDescriptor, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0)
        {
            std::cerr << "Receive failed!\n";
            break;
        }

        buffer[bytesReceived] = '\0';
        std::cout << buffer;
    }

    close(clientSocketFileDescriptor);
}

void net::client::__INIT_MESSAGE_LISTENER_THREAD__(int clientSocketFileDescriptor)
{
    std::thread workerThread(&net::client::__MESSAGE_LISTENER_THREAD__, this, clientSocketFileDescriptor);
    workerThread.detach();
}

char *net::client::getClientName(size_t *__clientNameSize)
{
    char *clientName = nullptr;
    size_t clientNameSize = 0;

    std::cout << "Name: ";
    ssize_t clientNameCount = getline(&clientName, &clientNameSize, stdin);
    
    if (__clientNameSize != nullptr)
        *__clientNameSize = clientNameCount;

    if (clientName[clientNameCount - 1] == '\n')
        clientName[clientNameCount - 1] = '\0';

    clientName[clientNameCount - 1] = ':';
    clientNameCount++;
    clientName[clientNameCount - 1] = '\0';

    return clientName;
}

bool net::client::__INIT__(void)
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