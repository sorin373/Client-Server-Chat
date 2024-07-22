#include "web_server.hpp"
#include "global.hpp"

#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace net;

char *route = nullptr;
bool changeRoute = false;

int web_server::POST_request_handler(char *buffer, SOCKET client_socket_FD, unsigned int bytes_in)
{
    if (changeRoute && buffer)
    {
        if (route != nullptr) 
        {
            delete[] route;
            route = nullptr;
        }

        char *ptr = new char[strlen(buffer) + 1];
        strcpy(ptr, buffer);

        if (ptr != nullptr)
            for (unsigned int i = 0, n = strlen(ptr); i < n; i++)
                if (ptr[i] == '/')
                {
                    route = new char[strlen(ptr + i) + 1];
                    strcpy(route, ptr + i);
                    break;
                }

        if (route != nullptr)
            for (unsigned int i = 0, n = strlen(route); i < n; i++)
                if (route[i] == ' ')
                {
                    route[i] = '\0';
                    break;
                }

        changeRoute = false;

        delete[] ptr;
    }

    if (user.routeManager(buffer, route, client_socket_FD, bytes_in) == EXIT_FAILURE) 
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int web_server::GET_request_handler(char *buffer, SOCKET client_socket_FD)
{
    bool USE_DEFAULT_ROUTE = false;

    const char defaultRoute[] = "interface/login.html";
    const char root[] = "interface";

    char *path = nullptr;

    if (buffer == nullptr) return EXIT_FAILURE;

    for (int i = 0, n = strlen(buffer); i < n; i++)
        if (buffer[i] == '/')
        {
            path = &buffer[i];
            break;
        }

    if (path == nullptr) USE_DEFAULT_ROUTE = true;

    std::ifstream file;

    if (!USE_DEFAULT_ROUTE)
    {
        for (int i = 0, n = strlen(path); i < n; i++)
            if (path[i] == ' ')
                path[i] = '\0';

        if ((strlen(path) == 1 && path[0] == '/')) USE_DEFAULT_ROUTE = true;

        if (strcmp(path, "/login.html") != 0 && strcmp(path, "/changePassword.html") != 0 && strcmp(path, "/createAccount.html") != 0 &&
            !strstr(path, ".css") && !strstr(path, ".png") && !user.getAuthStatus())
            USE_DEFAULT_ROUTE = true;

        if (!USE_DEFAULT_ROUTE)
        {
            if (strcmp(path, "/login.html") == 0)
            {
                user.resetAuthStatus();
                user.resetSessionID();
            }

            char fullPath[strlen(root) + strlen(path) + 1] = "";

            if (!strstr(path, "interface")) strcpy(fullPath, root);

            strcat(fullPath, path);

            file.open(fullPath, std::ios::binary);
        }
    }

    if (USE_DEFAULT_ROUTE) 
        file.open(defaultRoute, std::ios::binary);

    if (!file.is_open())
    {
        if (DEBUG_FLAG)
            std::cerr << std::setw(5) << " "
                      << "==F== Encountered an error while attempting to open the GET file (f_13)\n";
        return EXIT_FAILURE;
    }

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\nContent-Length: ";

    file.seekg(0, std::ios::end);
    int size = file.tellg();
    response << size << "\r\n\r\n";

    file.seekg(0, std::ios::beg);
    response << file.rdbuf();

    send_to_client(client_socket_FD, response.str().c_str(), response.str().size() + 1);

    return EXIT_SUCCESS;
}

char *requestType = nullptr;

int web_server::request_handler(char *buffer, SOCKET client_socket_FD, unsigned int bytes_in)
{
    char *copyBuffer = new char[bytes_in + 1];

    strncpy(copyBuffer, buffer, bytes_in);
    copyBuffer[bytes_in] = '\0';

    char *ptr = strstr(copyBuffer, "GET");

    if (ptr == NULL)
    {
        strncpy(copyBuffer, buffer, bytes_in);
        copyBuffer[bytes_in] = '\0';

        ptr = strstr(copyBuffer, "POST");
    }

    if (ptr != NULL)
    {
        if (requestType != nullptr) delete[] requestType;
        
        changeRoute = true;

        for (unsigned int i = 0; ptr[i] != '\0'; i++)
            if (ptr[i] == ' ')
            {
                ptr[i] = '\0';
                break;
            }   

        requestType = new char[strlen(ptr) + 1];

        strcpy(requestType, ptr);
    }

    if (requestType != nullptr && strcasecmp(requestType, "GET") == 0)
        if (GET_request_handler(buffer, client_socket_FD) == EXIT_FAILURE)
        {
            delete[] copyBuffer;
            return EXIT_FAILURE;
        }

    if (requestType != nullptr && strcasecmp(requestType, "POST") == 0)
        if (POST_request_handler(buffer, client_socket_FD, bytes_in) == EXIT_FAILURE)
        {
            delete[] copyBuffer;
            return EXIT_FAILURE;
        }

    delete[] copyBuffer;

    return EXIT_SUCCESS;
}

void web_server::on_client_connected(SOCKET client_socket)
{
    std::cout << "--> Client socket connected: " << client_socket << "\n";
}

void web_server::on_client_disconnected(SOCKET client_socket)
{
    std::cout << "--> Client socket disconnected: " << client_socket << " - Performing post receive scripts...\n" ;

    char response[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";
    std::string file = user.getFileInQueue();

    if (!file.empty())
    {   
        std::cout << std::setw(3) << " " << "| Formating file: '" << file << "'!\n"; 
        user.format_file(file);
        
        std::cout << std::setw(3) << " " << "| Adding '" << file << "' to the database!\n";
        if (user.addToFileTable(file.c_str(), TOTAL_BYTES_RECV / 1000000.0000) == -1)
            std::cerr << std::setw(3) << " " << "|==DB== Failed to add '" << file << "' to the database!\n";

        std::cout << std::setw(3) << " " << "| Clearing '" << file << "' from queue!\n";
        user.clearFileInQueue();

        TOTAL_BYTES_RECV = 0;

        std::cout << std::setw(3) << " " << "| Responding to client request!\n";
        send_to_client(client_socket, response, strlen(response) + 1);
    }
}

void web_server::on_message_received(SOCKET client_socket, char *msg, unsigned int size)
{
    request_handler(msg, client_socket, size);
}

int web_server::ws_init(int argc, char* argv[])
{   
    if (user.get_m_sql_db()->init() == -1)
        return -1;

    user.SQLfetchFileTable();

    user.SQLfetchUserTable();

    if (init() == -1)
        return -1;

    return EXIT_SUCCESS;
}