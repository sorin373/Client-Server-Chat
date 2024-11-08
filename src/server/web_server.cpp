#include "web_server.hpp"
#include "global.hpp"

#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>

using namespace net;

int web_server::POST_request_handler(char *buffer, SOCKET client_socket_FD, unsigned int bytes_in)
{
    if (this->m_change_route && buffer != nullptr)
    {
        if (this->m_current_route != nullptr)
            this->free_current_route();

        char *cbuff = static_cast<char*>(malloc((strlen(buffer) + 1) * sizeof(char)));
        strcpy(cbuff, buffer);

        if (cbuff != nullptr)
            for (unsigned int i = 0, n = strlen(cbuff); i < n; ++i)
                if (cbuff[i] == '/')
                {
                    this->m_current_route = static_cast<char*>(malloc((strlen(cbuff + i) + 1) * sizeof(char)));
                    strcpy(this->m_current_route, cbuff + i);

                    break;
                }

        if (this->m_current_route != nullptr)
            for (unsigned int i = 0, n = strlen(this->m_current_route); i < n; ++i)
                if (this->m_current_route[i] == ' ')
                {
                    this->m_current_route[i] = '\0';
                    break;
                }

        this->m_change_route = false;

        free(cbuff);
    }

    if (this->m_user.routeManager(buffer, this->m_current_route, client_socket_FD, bytes_in) == EXIT_FAILURE)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int web_server::GET_request_handler(char *buffer, SOCKET client_socket_FD)
{
    bool USE_DEFAULT_ROUTE = false;

    constexpr char DEFAULT_ROUTE[] = "interface/login.html";
    constexpr char root[] = "interface";

    char *path = nullptr;

    if (buffer == nullptr)
        return EXIT_FAILURE;

    for (unsigned int i = 0, n = strlen(buffer); i < n; ++i)
        if (buffer[i] == '/')
        {
            path = buffer + i;
            break;
        }

    if (path == nullptr)
        USE_DEFAULT_ROUTE = true;

    std::ifstream file;

    if (!USE_DEFAULT_ROUTE)
    {
        for (unsigned int i = 0, n = strlen(path); i < n; ++i)
            if (path[i] == ' ')
                path[i] = '\0';

        if ((strlen(path) == 1 && path[0] == '/'))
            USE_DEFAULT_ROUTE = true;
        else if (strcmp(path, "/login.html") != 0 && strcmp(path, "/changePassword.html") != 0 && strcmp(path, "/createAccount.html") != 0 &&
            !strstr(path, ".css") && !strstr(path, ".png") && !m_user.getAuthStatus())
            USE_DEFAULT_ROUTE = true;

        if (!USE_DEFAULT_ROUTE)
        {
            if (strcmp(path, "/login.html") == 0)
            {
                this->m_user.resetAuthStatus();
                this->m_user.resetSessionID();
            }

            char full_path[strlen(root) + strlen(path) + 1] = "";

            if (!strstr(path, "interface"))
                strcpy(full_path, root);

            strcat(full_path, path);

            file.open(full_path, std::ios::binary);
        }
    }

    if (USE_DEFAULT_ROUTE)
        file.open(DEFAULT_ROUTE, std::ios::binary);

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
    unsigned int size = file.tellg();

    response << size << "\r\n\r\n";

    file.seekg(0, std::ios::beg);

    response << file.rdbuf();

    this->send_to_client(client_socket_FD, response.str().c_str(), response.str().size() + 1);

    return EXIT_SUCCESS;
}

int web_server::request_handler(char *buffer, SOCKET client_socket_FD, unsigned int bytes_in)
{
    if (buffer == nullptr || bytes_in == 0)
        return EXIT_FAILURE;

    char *cbuff = static_cast<char *>(malloc((bytes_in + 1) * sizeof(char)));

    strncpy(cbuff, buffer, bytes_in);
    cbuff[bytes_in] = '\0';

    char *ptr = strstr(cbuff, "GET");

    if (ptr == nullptr)
        ptr = strstr(cbuff, "POST");

    if (ptr != nullptr)
    {
        this->free_request_type();

        this->m_change_route = true;

        for (unsigned int i = 0; ptr[i] != '\0'; ++i)
            if (ptr[i] == ' ')
            {
                ptr[i] = '\0';
                break;
            }

        this->m_request_type = static_cast<char *>(malloc((strlen(ptr) + 1) * sizeof(char)));
        strcpy(this->m_request_type, ptr);
    }

    if (this->m_request_type != nullptr && strcasecmp(this->m_request_type, "GET") == 0)
    {
        if (GET_request_handler(buffer, client_socket_FD) == EXIT_FAILURE)
        {
            free(cbuff);
            return EXIT_FAILURE;
        }
    }
    else if (this->m_request_type != nullptr && strcasecmp(this->m_request_type, "POST") == 0)
    {
        if (POST_request_handler(buffer, client_socket_FD, bytes_in) == EXIT_FAILURE)
        {
            free(cbuff);
            return EXIT_FAILURE;
        }
    }

    free(cbuff);

    return EXIT_SUCCESS;
}

void web_server::on_client_disconnected(SOCKET client_socket)
{
    std::cout << "--> Client socket disconnected: " << client_socket << " - Performing post receive scripts...\n";

    char response[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";
    std::string file = m_user.getFileInQueue();

    if (!file.empty())
    {
        std::cout << std::setw(3) << " " << "| Formating file: '" << file << "'!\n";
        m_user.format_file(file);

        std::cout << std::setw(3) << " " << "| Adding '" << file << "' to the database!\n";

        if (m_user.addToFileTable(file.c_str(), TOTAL_BYTES_RECV / 1000000.0000) == -1)
            std::cerr << std::setw(3) << " " << "|==DB== Failed to add '" << file << "' to the database!\n";

        std::cout << std::setw(3) << " " << "| Clearing '" << file << "' from queue!\n";
        m_user.clearFileInQueue();

        TOTAL_BYTES_RECV = 0;

        std::cout << std::setw(3) << " " << "| Responding to client request!\n";

        send_to_client(client_socket, response, strlen(response) + 1);
    }
}

void web_server::on_message_received(SOCKET client_socket, char *msg, unsigned int size)
{
    request_handler(msg, client_socket, size);
}

int web_server::ws_init(int argc, char *argv[])
{
    char connection_name[MAX_L], username[MAX_L], password[MAX_L], schema[MAX_L];

    std::cout << "Hostname: ";
    std::cin.get(connection_name, MAX_L);
    std::cin.get();

    std::cout << "\nUsername: ";
    std::cin.get(username, MAX_L);
    std::cin.get();

    std::cout << "\nPassword: ";
    std::cin.get(password, MAX_L);
    std::cin.get();

    std::cout << "\nSchema: ";
    std::cin.get(schema, MAX_L);

    if (m_user.mysql_handler()->init(connection_name, username, password, schema) == -1)
        return -1;

    m_user.SQLfetchFileTable();

    m_user.SQLfetchUserTable();

    if (init() == -1)
        return -1;

    return EXIT_SUCCESS;
}