#include "socket/socketUtils.hpp"

#include <iostream>
#include <cstring>

int main()
{
    net::SocketUtils *socket = new net::SocketUtils;

    int socketFD = socket->createSocket();
    
    if (socketFD == -1) {
        std::cerr << "Socket creation failed\n";
    }
    
    struct sockaddr_in *address = socket->IPv4Address("142.250.188.46", 80);

    int res = socket->connectToServer(socketFD, address);

    if (res == -1) {
        std::cerr << "Connection failed\n";
        free(address); // Clean up allocated memory
        delete socket;
        return 1;
    }

    std::cout << "Successful connection!\n";
    
    char buffer[20 * 1024];
    const char msg[] = "GET / HTTP/1.1\r\nHost: google.com\r\n\r\n";
    send(socketFD, msg, strlen(msg), 0);

    ssize_t bytes_received = recv(socketFD, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        std::cerr << "Receive failed\n";
    } else {
        buffer[bytes_received] = '\0'; // Ensure null-termination for printing
        std::cout << "Received:\n" << buffer << std::endl;
    }

    free(address); // Clean up allocated memory
    delete socket;

    return 0;
}
