#include "serverUtils.hpp"

#include <iostream>
#include <cstring>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>

net::server::server(const int clientSocketFileDescriptor)
{
    this->clientSocketFileDescriptor = clientSocketFileDescriptor;
}

template <typename T> void net::server::acceptConnection(const int serverSocketFileDescriptor, struct acceptedSocket<T> *__acceptedSocket)
{
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);

    int clientSocketFD = accept(serverSocketFileDescriptor, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddressSize);

    __acceptedSocket->getAcceptedSocket(clientAddress, clientSocketFD, clientSocketFD > 0, clientSocketFD <= 0);
}

template <typename T> void net::server::handleGETrequests(const T *buffer, int acceptedSocketFileDescriptor)
{
    for (auto &socket : connectedSockets)
    {
        int socketFD = socket.getAcceptedSocketFileDescriptor();

        if (socketFD == acceptedSocketFileDescriptor)
            send(socketFD, buffer, strlen(buffer), 0);
    }
}

template <typename T> void net::server::sendReceivedMessage(T *buffer, int acceptedSocketFileDescriptor)
{
    for (auto &socket : connectedSockets)
    {
        int socketFD = socket.getAcceptedSocketFileDescriptor();

        if (socketFD != acceptedSocketFileDescriptor)
            send(socketFD, buffer, strlen(buffer), 0);
    }
}

template <typename T> void net::server::printReceivedData(struct acceptedSocket<T> *socket)
{
    T buffer[1025];

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

        socket->addHTTPcontent(*buffer);

        

        net::server::sendReceivedMessage(buffer, acceptedSocketFD);
    }

    close(socket->getAcceptedSocketFileDescriptor());
}

template <typename T> void net::server::printReceivedDataThread(struct acceptedSocket<T> *psocket)
{
    std::thread printThread(&server::printReceivedData<char>, this, psocket);
    printThread.detach();
}

void net::server::handleClientConnections(int serverSocketFileDescriptor)
{
    while (true)
    {
        server::acceptedSocket<char> *newAcceptedSocket = new server::acceptedSocket<char>();

        acceptConnection(serverSocketFileDescriptor, newAcceptedSocket);

        connectedSockets.push_back(*newAcceptedSocket);

        printReceivedDataThread(newAcceptedSocket);
    }

    close(serverSocketFileDescriptor);
}

void net::server::__MASTER_THREAD__(int serverSocketFileDescriptor)
{
    std::thread workerThread(&server::handleClientConnections, this, serverSocketFileDescriptor);
    workerThread.join();
}

int net::server::bindServer(int serverSocketFileDescriptor, struct sockaddr_in *serverAddress)
{
    return bind(serverSocketFileDescriptor, (struct sockaddr *)serverAddress, sizeof(struct sockaddr_in));
}

int net::server::getClientSocketFileDescriptor(void) const noexcept
{
    return clientSocketFileDescriptor;
}

template <typename S> net::server::acceptedSocket<S>::acceptedSocket() {}

template <typename S> void net::server::acceptedSocket<S>::getAcceptedSocket
    (const struct sockaddr_in ipAddress, const int acceptedSocketFileDescriptor, const bool acceptStatus, const int error)
{
    this->ipAddress = ipAddress;
    this->acceptedSocketFileDescriptor = acceptedSocketFileDescriptor;
    this->acceptStatus = acceptStatus;
    this->error = error;
}

template <typename S> void net::server::acceptedSocket<S>::addHTTPcontent(const S buffer) noexcept
{
    content.push_back(buffer);
}

template <typename S> struct sockaddr_in net::server::acceptedSocket<S>::getIpAddress(void) const noexcept
{
    return ipAddress;
}

template <typename S> int net::server::acceptedSocket<S>::getError(void) const noexcept
{
    return error;
}

template <typename S> bool net::server::acceptedSocket<S>::getAcceptStatus(void) const noexcept
{
    return acceptStatus;
}

template <typename S> int net::server::acceptedSocket<S>::getAcceptedSocketFileDescriptor(void) const noexcept
{
    return acceptedSocketFileDescriptor;
}

template <typename S> std::string net::server::acceptedSocket<S>::getContent(void) const noexcept
{
    return content;
}

template <typename S> std::vector<struct net::server::acceptedSocket<S>> net::server::getConnectedSockets(void) const noexcept
{
    return connectedSockets;
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

    struct sockaddr_in *serverAddress = serverSocket->IPv4Address();

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

    __server->__MASTER_THREAD__(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);
    free(serverAddress);
    delete serverSocket;
    delete __server;

    return EXIT_SUCCESS;
}