#include "serverUtils.hpp"

#include <iostream>
#include <cstring>
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

void net::server::sendReceivedMessage(char *buffer, int acceptedSocketFileDescriptor)
{
    for (auto &socket : connectedSockets)
    {
        int socketFD = socket.getAcceptedSocketFileDescriptor();
        
        if (socketFD != acceptedSocketFileDescriptor)
            send(socketFD, buffer, strlen(buffer), 0);
    }
}

void net::server::printReceivedData(const struct acceptedSocket *socket)
{
    char buffer[1024];

    while (true)
    {
        int acceptedSocketFD = socket->getAcceptedSocketFileDescriptor();
        ssize_t bytesReceived = recv(acceptedSocketFD, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0)
        {
            std::cerr << "Receive failed! "
                      << socket->getError() << "\n";
            break;
        }

        buffer[bytesReceived] = '\0';
        std::cout << buffer;

        net::server::sendReceivedMessage(buffer, acceptedSocketFD);
    }

    close(socket->getAcceptedSocketFileDescriptor());
}

void net::server::__PRINT_THREAD__(struct acceptedSocket *psocket)
{
    std::thread printThread(&server::printReceivedData, this, psocket);
    printThread.detach();
}

void net::server::__SERVER_THREAD__(int serverSocketFileDescriptor)
{
    while (true)
    {
        server::acceptedSocket *newAcceptedSocket = new server::acceptedSocket();

        acceptConnection(serverSocketFileDescriptor, newAcceptedSocket);

        connectedSockets.push_back(*newAcceptedSocket);
        
        __PRINT_THREAD__(newAcceptedSocket);
    }

    close(serverSocketFileDescriptor);
}

void net::server::__INIT_SERVER_THREAD__(int serverSocketFileDescriptor)
{
    std::thread workerThread(&server::__SERVER_THREAD__, this, serverSocketFileDescriptor);
    workerThread.join();
}

int net::server::bindServer(int serverSocketFileDescriptor, struct sockaddr_in *serverAddress)
{
    return bind(serverSocketFileDescriptor, (struct sockaddr *)serverAddress, sizeof(struct sockaddr_in));
}

int net::server::acceptedSocket::getAcceptedSocketFileDescriptor(void) const noexcept
{
    return acceptedSocketFileDescriptor;
}

std::vector<struct net::server::acceptedSocket> net::server::getConnectedSockets(void) const noexcept
{
    return connectedSockets;
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

bool net::server::__INIT__(void)
{
    net::SocketUtils *serverSocket = new net::SocketUtils;

    int serverSocketFD = serverSocket->createSocket();

    if (serverSocketFD == -1)
    {
        std::cerr << "Failed to create socket!\n";
        delete serverSocket;

        return EXIT_FAILURE;
    }

    net::server *__server = new net::server(serverSocketFD);

    struct sockaddr_in *serverAddress = serverSocket->IPv4Address("", PORT);

    int res = __server->bindServer(serverSocketFD, serverAddress);
    if (res == 0)
        std::cout << "Server socket bound successfully!\n";

    int listenResult = listen(serverSocketFD, 10);
    if (listenResult == -1)
    {
        shutdown(serverSocketFD, SHUT_RDWR);
        free(serverAddress);
        delete serverSocket;
        delete __server;

        return EXIT_FAILURE;
    }

    __server->__INIT_SERVER_THREAD__(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);
    free(serverAddress);
    delete serverSocket;
    delete __server;

    return EXIT_SUCCESS;
}