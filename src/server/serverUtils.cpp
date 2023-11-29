#include "serverUtils.hpp"

#include <iostream>
#include <thread>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

net::server::server(const int clientSocketFileDescriptor)
{
    this->clientSocketFileDescriptor = clientSocketFileDescriptor;
}

void net::server::acceptedSocket::getAcceptedSocket(const struct sockaddr_in ipAddress, const int acceptedSocketFileDescriptor, const bool acceptStatus, const int error)
{
    this->ipAddress = ipAddress;
    this->acceptedSocketFileDescriptor = acceptedSocketFileDescriptor;
    this->acceptStatus = acceptStatus;
    this->error = error;
}

void net::server::acceptConnection(const int serverSocketFileDescriptor, struct acceptedSocket *__acceptedSocket)
{
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    int clientSocketFD = accept(serverSocketFileDescriptor, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddressSize);

    __acceptedSocket->getAcceptedSocket(clientAddress, clientSocketFD, clientSocketFD > 0, clientSocketFD <= 0);
}

void net::server::printReceivedData(const struct acceptedSocket *socket) const noexcept
{
    char buffer[1024];

    while (true)
    {
        ssize_t bytes_received = recv(socket->getAcceptedSocketFileDescriptor(), buffer, sizeof(buffer), 0);

        if (bytes_received <= 0)
        {
            std::cerr << "Receive failed! "
                      << socket->getError() << "\n";
            break;
        }

        buffer[bytes_received] = '\0';
        std::cout << "Received:\n"
                  << buffer << std::endl;
    }
}

void net::server::__SERVER_THREAD__(int serverSocketFileDescriptor)
{
    server::acceptedSocket *newAcceptedSocket = new server::acceptedSocket();

    acceptConnection(serverSocketFileDescriptor, newAcceptedSocket);
    printReceivedData(newAcceptedSocket);
    close(newAcceptedSocket->getAcceptedSocketFileDescriptor());
    
    delete newAcceptedSocket;
}

void net::server::__INIT_SERVER_THREAD__(int serverSocketFileDescriptor)
{
    std::thread workerThread(&server::__SERVER_THREAD__, this, serverSocketFileDescriptor);
    workerThread.detach();
}

int net::server::acceptedSocket::getAcceptedSocketFileDescriptor(void) const noexcept
{
    return acceptedSocketFileDescriptor;
}

int net::server::acceptedSocket::getError(void) const noexcept
{
    return error;
}

bool net::server::acceptedSocket::getAcceptStatus(void) const noexcept
{
    return acceptStatus;
}

int net::server::getClientSocketFileDescriptor(void) const noexcept
{
    return clientSocketFileDescriptor;
}

struct sockaddr_in net::server::acceptedSocket::getIpAddress(void) const noexcept
{
    return ipAddress;
}
