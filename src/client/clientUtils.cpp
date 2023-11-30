#include "clientUtils.hpp"

#include <iostream>
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