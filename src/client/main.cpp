#include "../socket/socketUtils.hpp"
#include "clientUtils.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>

int main()
{
    net::SocketUtils *socket = new net::SocketUtils;
    net::client *client = new net::client;

    int socketFD = socket->createSocket();

    if (socketFD == -1)
    {
        std::cerr << "Socket creation failed\n";
        return EXIT_FAILURE;
    }

    struct sockaddr_in *address = socket->IPv4Address(net::localHostAddress, net::PORT);

    int res = socket->connectToServer(socketFD, address);

    if (res == -1)
    {
        std::cerr << "Connection failed\n";
        free(address);
        delete socket;
        return EXIT_FAILURE;
    }

    std::cout << "Connection established!\n";

    char *buffer = nullptr;
    size_t bufferSize = 0;

    char *clientName = client->getClientName();

    client->__INIT_MESSAGE_LISTENER_THREAD__(socketFD);

    std::ifstream htmlFile("index.html");

    if (!htmlFile.is_open())
    {
        std::cerr << "Failed to open file!\n";
        return EXIT_FAILURE;
    }

    std::stringstream hBuffer;
    hBuffer << htmlFile.rdbuf();
    htmlFile.close();
    std::string scontent = hBuffer.str();
    

    const char *content = scontent.c_str(); 

    const char* response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: %zu\r\n"
        "\r\n"
        "%s";

    size_t contentLength = strlen(content);
    size_t responseLength = snprintf(nullptr, 0, response, contentLength, content);

    char* httpResponse = new char[responseLength + 1];

    snprintf(httpResponse, responseLength + 1, response, contentLength, content);

    ssize_t sent = client->sendData<char>(socketFD, httpResponse, responseLength);

    while (true)
    {
        
        
        // std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        
        /*ssize_t charCount = getline(&buffer, &bufferSize, stdin);

        strcat(clientName, buffer);

        if (charCount > 0)
        {
            if (strcasecmp(buffer, "exit\n") == 0)
                break;

            //ssize_t amountSent = client->sendData<char>(socketFD, clientName, strlen(clientName) + 1);
            
        }*/
        
    }

    htmlFile.close();

    close(socketFD);

    free(address);
    delete socket;
    delete[] httpResponse;

    return EXIT_SUCCESS;
}
