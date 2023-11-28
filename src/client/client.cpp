#include "../socket/socketUtils.hpp"

#include <iostream>
#include <cstring>
#include <stdio.h>
#include <unistd.h>

int main()
{
    net::SocketUtils *socket = new net::SocketUtils;

    int socketFD = socket->createSocket();
    
    if (socketFD == -1) {
        std::cerr << "Socket creation failed\n";
        return EXIT_FAILURE;
    }
    
    struct sockaddr_in *address = socket->IPv4Address(localHostAddress, 2000);

    int res = socket->connectToServer(socketFD, address);

    if (res == -1) {
        std::cerr << "Connection failed\n";
        free(address);
        delete socket;
        return EXIT_FAILURE;
    }

    std::cout << "Successful connection!\n";
    
    char *buffer = nullptr;
    size_t bufferSize = 0;
    
    while (true)
    {
        ssize_t charCount = getline(&buffer, &bufferSize, stdin);

        if (charCount > 0)
        {
            if (strcasecmp(buffer, "exit\n") == 0)
                break;

            ssize_t amountSent = send(socketFD, buffer, charCount, 0);
        }
    }

    close(socketFD);

    free(address);
    delete socket;

    return EXIT_SUCCESS;
}
