#include "server.hpp"
#include "../socket/socketUtils.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>

int main()
{
    net::SocketUtils *serverSocket = new net::SocketUtils;
    int serverSocketFD = serverSocket->createSocket();
    
    if (serverSocketFD == -1)
    {
        std::cerr << "Socket creation failed\n";
        return EXIT_FAILURE;
    }

    struct sockaddr_in *serverAddress = serverSocket->IPv4Address("", 2000);

    int res = bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(struct sockaddr_in));
    if (res == 0)
        std::cout << "Socket was bound successfully!\n";

    int listenRes = listen(serverSocketFD, 10);

    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);
    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddressSize);

    char buffer[20 * 1024];
    size_t bytes_received = recv(clientSocketFD, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        std::cerr << "Receive failed\n";
    } else {
        std::cout << "Received:\n" << buffer << std::endl;
    }

    free(serverAddress);
    delete serverSocket;
}