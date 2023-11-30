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