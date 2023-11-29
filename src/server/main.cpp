#include "serverUtils.hpp"
#include "../socket/socketUtils.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main()
{
    net::SocketUtils *serverSocket = new net::SocketUtils;
    
    int serverSocketFD = serverSocket->createSocket();

    if (serverSocketFD == -1)
    {
        std::cerr << "Socket creation failed\n";
        return EXIT_FAILURE;
    }

    net::server *__server = new net::server(serverSocketFD);

    struct sockaddr_in *serverAddress = serverSocket->IPv4Address("", PORT);

    int res = bind(serverSocketFD, (struct sockaddr *)serverAddress, sizeof(struct sockaddr_in));
    if (res == 0)
        std::cout << "Socket was bound successfully!\n";

    int listenRes = listen(serverSocketFD, 10);
    
    net::server::acceptedSocket *__acceptedSocket = new net::server::acceptedSocket;
    
    __server->acceptConnection(serverSocketFD, __acceptedSocket);
    __server->printReceivedData(__acceptedSocket);

    close(__acceptedSocket->getAcceptedSocketFileDescriptor());
    shutdown(serverSocketFD, SHUT_RDWR);
    free(serverAddress);
    delete serverSocket;
    delete __server;
    delete __acceptedSocket;

    return EXIT_SUCCESS;
}