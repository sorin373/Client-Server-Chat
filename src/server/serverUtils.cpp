#include "serverUtils.hpp"

#include <iostream>
#include <cstring>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include "interface/interface.hpp"
#include "database/database.hpp"
#include <mysql_connection.h>
#include <mysql_driver.h>

net::server::server(const int clientSocketFileDescriptor)
{
    this->clientSocketFileDescriptor = clientSocketFileDescriptor;
    this->db = nullptr;
}

template <typename T>
void net::server::acceptConnection(const int serverSocketFileDescriptor, class acceptedSocket<T> *__acceptedSocket)
{
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);

    int clientSocketFD = accept(serverSocketFileDescriptor, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddressSize);

    __acceptedSocket->getAcceptedSocket(clientAddress, clientSocketFD, clientSocketFD > 0, clientSocketFD <= 0);
}

template <typename T>
int net::server::handleGETrequests(T *buffer, int acceptedSocketFileDescriptor)
{
    char *allocatedBuffer = buffer;
    char *copyBuffer = new char[strlen(buffer) + 1];
    strcpy(copyBuffer, allocatedBuffer);

    char *path = nullptr;

    for (int i = 0, n = strlen(allocatedBuffer); i < n; i++)
        if (allocatedBuffer[i] == '/')
        {
            path = &allocatedBuffer[i];
            break;
        }

    for (int i = 0, n = strlen(path); i < n; i++)
        if (path[i] == ' ')
            path[i] = '\0';

    if ((strlen(path) == 1 && path[0] == '/') || path == nullptr)
        strcpy(path, "/login.html");
    
    if (allocatedBuffer[0] == 'P')
        net::interface::routeHandler(copyBuffer);

    const char root[] = "interface";
    char fullPath[strlen(root) + strlen(path) + 1];
    strcpy(fullPath, root);
    strcat(fullPath, path);

    std::cout << "FULL PATH: " << fullPath << "\n\n";

    std::ifstream file(fullPath, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Failed to open " << fullPath << "!\n";
        return EXIT_FAILURE;
    }

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\nContent-Length: ";

    file.seekg(0, std::ios::end);
    int size = file.tellg();
    response << size << "\r\n\r\n";

    file.seekg(0, std::ios::beg);
    response << file.rdbuf();

    if (send(acceptedSocketFileDescriptor, response.str().c_str(), response.str().size(), 0) == -1)
    {
        std::cerr << "Failed to send response.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

template <typename T>
void net::server::sendReceivedMessage(T *buffer, int acceptedSocketFileDescriptor)
{
    for (auto &socket : connectedSockets)
    {
        int socketFD = socket.getAcceptedSocketFileDescriptor();

        if (socketFD != acceptedSocketFileDescriptor)
            send(socketFD, buffer, strlen(buffer), 0);
    }
}

template <typename T>
void net::server::printReceivedData(class acceptedSocket<T> *socket)
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

        std::cout << "------------------------------------------------------------------------------------------\n\n";

        buffer[bytesReceived] = '\0';
        std::cout << buffer;

        handleGETrequests(buffer, acceptedSocketFD);

        // net::server::sendReceivedMessage(buffer, acceptedSocketFD);
    }

    close(socket->getAcceptedSocketFileDescriptor());
}

template <typename T>
void net::server::printReceivedDataThread(class acceptedSocket<T> *psocket)
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

template <typename S>
net::server::acceptedSocket<S>::acceptedSocket() {}

template <typename S>
void net::server::acceptedSocket<S>::getAcceptedSocket(const struct sockaddr_in ipAddress, const int acceptedSocketFileDescriptor, const bool acceptStatus, const int error)
{
    this->ipAddress = ipAddress;
    this->acceptedSocketFileDescriptor = acceptedSocketFileDescriptor;
    this->acceptStatus = acceptStatus;
    this->error = error;
}

template <typename S>
struct sockaddr_in net::server::acceptedSocket<S>::getIpAddress(void) const noexcept
{
    return ipAddress;
}

template <typename S>
int net::server::acceptedSocket<S>::getError(void) const noexcept
{
    return error;
}

template <typename S>
bool net::server::acceptedSocket<S>::getAcceptStatus(void) const noexcept
{
    return acceptStatus;
}

template <typename S>
int net::server::acceptedSocket<S>::getAcceptedSocketFileDescriptor(void) const noexcept
{
    return acceptedSocketFileDescriptor;
}

template <typename S>
std::vector<class net::server::acceptedSocket<S>> net::server::getConnectedSockets(void) const noexcept
{
    return connectedSockets;
}

bool net::server::__database_init__(void)
{
    if (db != nullptr)
    {
        std::cerr << "Database is live!\n";
        return EXIT_SUCCESS;
    }

    sql::Driver     *driver = nullptr;
    sql::Connection *con = nullptr;

    driver = sql::mysql::get_mysql_driver_instance();

    con = driver->connect();

    if (con == nullptr)
    {
        std::cerr << "";
        return EXIT_FAILURE;
    }

    con->getSchema("");

    db = new net::server::database(driver, con, true);

    return EXIT_SUCCESS;
}

bool net::server::__INIT__(char *portArg)
{
    int port = 0;

    if (portArg == nullptr)
        port = DEFAULT_PORT;
    else
        port = atoi(portArg);

    net::SocketUtils *serverSocket = new net::SocketUtils;

    int serverSocketFD = serverSocket->createSocket();

    if (serverSocketFD == -1)
    {
        std::cerr << "Failed to create socket!\n";
        delete serverSocket;

        return EXIT_FAILURE;
    }

    net::server *__server = new net::server(serverSocketFD);

    std::cout << serverSocket->getMachineIPv4Address() << ":" << port << "\n";

    struct sockaddr_in *serverAddress = serverSocket->IPv4Address(serverSocket->getMachineIPv4Address(), port);

    int res = __server->bindServer(serverSocketFD, serverAddress);
    if (res == 0)
        std::cout << "Server socket bound successfully!\n";
    else
    {
        std::cerr << "Error binding the server!\n";
        perror("bind");
        return EXIT_FAILURE;
    }

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