#include "web_server.hpp"

#include <cstring>
#include <iostream>
#include <fstream>
#include <iomanip>

namespace net
{
    HTTP_STATUS web_server::POST_request_handler(char *buffer, SOCKET client_socket_FD, unsigned int bytes_in)
    {
        if (buffer == nullptr)
            return HTTP_STATUS::BAD_REQUEST;

        if (this->m_change_route)
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

        if (this->route_manager(buffer, this->m_current_route, client_socket_FD, bytes_in) != HTTP_STATUS::SUCCESS)
            return HTTP_STATUS::SERVER_ERROR;

        return HTTP_STATUS::SUCCESS;
    }

    HTTP_STATUS web_server::GET_request_handler(char *buffer, SOCKET client_socket_FD)
    {
        bool USE_DEFAULT_ROUTE = false;

        char *path = nullptr;

        if (buffer == nullptr)
            return HTTP_STATUS::BAD_REQUEST;

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

            if (!USE_DEFAULT_ROUTE)
            {
                char full_path[strlen(ROOT) + strlen(path) + 1] = "";

                if (!strstr(path, "interface"))
                    strcpy(full_path, ROOT);

                strcat(full_path, path);

                file.open(full_path, std::ios::binary);
            }
        }

        if (USE_DEFAULT_ROUTE)
            file.open(DEFAULT_ROUTE, std::ios::binary);

        if (!file.is_open())
        {
            std::cerr << std::setw(5) << " "
                      << "==F== Encountered an error while attempting to open the GET file (f_13)\n";
            return HTTP_STATUS::SERVER_ERROR;
        }

        std::ostringstream response;
        response << "HTTP/1.1 200 OK\r\nContent-Length: ";

        file.seekg(0, std::ios::end);
        unsigned int size = file.tellg();

        response << size << "\r\n\r\n";

        file.seekg(0, std::ios::beg);

        response << file.rdbuf();

        this->send_to_client(client_socket_FD, response.str().c_str(), response.str().size() + 1);

        return HTTP_STATUS::SUCCESS;
    }

    HTTP_STATUS web_server::request_handler(char *buffer, SOCKET client_socket_FD, unsigned int bytes_in)
    {
        if (buffer == nullptr || bytes_in == 0)
            return HTTP_STATUS::BAD_REQUEST;

        // bytes + 1 in order to make room for nullptr
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

            this->m_request_type = static_cast<char*>(malloc((strlen(ptr) + 1) * sizeof(char)));
            strcpy(this->m_request_type, ptr);
        }

        if (this->m_request_type != nullptr && strcasecmp(this->m_request_type, "GET") == 0)
        {
            if (GET_request_handler(buffer, client_socket_FD) != HTTP_STATUS::SUCCESS)
            {
                free(cbuff);
                return HTTP_STATUS::SERVER_ERROR;
            }
        }
        else if (this->m_request_type != nullptr && strcasecmp(this->m_request_type, "POST") == 0)
        {
            if (POST_request_handler(buffer, client_socket_FD, bytes_in) != HTTP_STATUS::SUCCESS)
            {
                free(cbuff);
                return HTTP_STATUS::SERVER_ERROR;
            }
        }

        free(cbuff);

        return HTTP_STATUS::SUCCESS;
    }

    void web_server::on_message_received(SOCKET client_socket, char *msg, unsigned int size)
    {
        HTTP_STATUS status = request_handler(msg, client_socket, size);

        switch (status)
        {
        case HTTP_STATUS::SUCCESS:
            break;
        case HTTP_STATUS::BAD_REQUEST:
            std::cerr << "400 Bad Request.\n";
            this->send_to_client(client_socket, "400 Bad Request", 16);
            break;
        case HTTP_STATUS::SERVER_ERROR:
            std::cerr << "500 Internal Server Error.\n";
            this->send_to_client(client_socket, "500 Internal Server Error", 26);
            break;
        default:
            std::cerr << "500 Internal Server Error (Unhandled case).\n";
            this->send_to_client(client_socket, "500 Internal Server Error", 26);
            break;        
        }
    }

    int web_server::ws_easy_init()
    {
        if (this->tcp_easy_init() == -1)
            return -1;

        if (this->run() == -1)
            return -1;

        return 0;
    }
}