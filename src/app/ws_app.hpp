#pragma once

#include "../server_api/web_server.hpp"
#include "../server_api/sconfig.hpp"
#include "back-end/database.hpp"

#include <vector>
#include <string>
#include <cstring>
#include <netinet/in.h>

namespace ws_app
{
    using namespace net;

    struct user_credentials
    {
        user_credentials() noexcept
            : m_username(nullptr), m_password(nullptr), m_id() {}

        explicit user_credentials(const char *username, const char *password, const int id)
            : m_username(nullptr), m_password(nullptr), m_id(id)
        {
            if (username == nullptr || password == nullptr)
                throw std::invalid_argument("Table field is nullptr\n");

            this->m_username = strdup(username);
            this->m_password = strdup(password);
        }

        user_credentials(const user_credentials &other)
            : user_credentials(other.m_username, other.m_password, other.m_id) {}

        user_credentials &operator=(const user_credentials &) = delete;

        ~user_credentials()
        {
            if (this->m_username != nullptr)
            {
                free(this->m_username);
                this->m_username = nullptr;
            }

            if (this->m_password != nullptr)
            {
                free(this->m_password);
                this->m_password = nullptr;
            }
        }

        const char *password() const noexcept { return this->m_password; }

        const char *username() const noexcept { return this->m_username; }

        const int id() const noexcept { return this->m_id; }

    private:
        char *m_username, *m_password;
        int m_id;
    };

    struct user_files
    {
        user_files() noexcept
            : m_file_name(nullptr), m_date(nullptr), m_userID(), m_fileID(), m_file_size() {}

        user_files(const char *file_name, const int userID, const int fileID, const double file_size, const char *date)
            : m_file_name(nullptr), m_date(nullptr), m_userID(userID), m_fileID(fileID), m_file_size(file_size)
        {
            if (file_name == nullptr || date == nullptr)
                throw std::invalid_argument("Table field is nullptr\n");

            this->m_file_name = strdup(file_name);
            this->m_date = strdup(date);
        }

        user_files(const user_files &other)
            : user_files(other.m_file_name, other.m_userID, other.m_fileID, other.m_file_size, other.m_date) {}

        user_files &operator=(const user_files &) = delete;

        ~user_files()
        {
            if (this->m_file_name != nullptr)
            {
                free(this->m_file_name);
                this->m_file_name = nullptr;
            }

            if (this->m_date != nullptr)
            {
                free(this->m_date);
                this->m_date = nullptr;
            }
        }

        const char *file_name() const noexcept { return this->m_file_name; }

        const char *date() const noexcept { return this->m_date; }

        const int userID() const noexcept { return this->m_userID; }

        const int fileID() const noexcept { return this->m_fileID; }

        const double file_size() const noexcept { return this->m_file_size; }

    private:
        char *m_file_name, *m_date;
        int m_userID, m_fileID;
        double m_file_size;
    };

    class core : public net::web_server
    {   
    public:
        core() noexcept
            : web_server(), AUTH_STATUS(false), SESSION_ID(-1), TOTAL_BYTES_RECV(0.0) { }

        explicit core(const char *ip_address, int port)
            : web_server(ip_address, port), AUTH_STATUS(false), SESSION_ID(-1), TOTAL_BYTES_RECV(0.0) { }

        ~core()
        {
            this->uc.clear();
            this->uf.clear();
        }

        int app_easy_start();

    protected:
        virtual HTTP_STATUS route_manager(void *buffer, const char *route, SOCKET client_socket_FD, unsigned int bytes_in) override;

        virtual HTTP_STATUS GET_request_handler(char *buffer, SOCKET client_socket_FD) override;

        /**
        * @brief This function performs post receive operations such as:
        *        file formatting, adding the file metadata to the database and clearing the file from the queue.
        * @param client_socket The file descriptor for the accepted socket connection used when seding the HTTP response.
        */
        virtual void on_client_disconnected(SOCKET client_socket) override;

    private:
        /**
         * @brief This function validates user credentials.
         * @return Returns true on success and false if the user credentials are not valid.
         */
        bool check_credentials(const char username[], const char password[]);

        /**
         * @brief This function searches for a username if it has already been used.
         * @return Returns true on success and false if the username is not found.
         */
        bool find_username(const char username[]);

        int format_file(const std::string &file_name);

        /**
         * @brief This function is used to dynamically build 'index.html' during runtime.
         *        When a new file is uploaded to the Server the page is rebuilt, generating a number of <tr> elements equal to the number of files sent.
         */
        void build_index();

        // This function retrieves the "user" table from the database
        void SQLfetch_user_table();

        // This function retrieves the "file" table from the database
        void SQLfetch_file_table();

        /**
         * @brief This function adds the files uploaded to the Server.
         * @return Returns 0 on success, 1 for errors.
         */
        int add_to_file_table(const char *fileName, const double fileSize);
        
        /**
         *
         * @brief This function handles the '/userlogin' route, sending an appropriate HTTP response to the client.
         *
         * @param buffer Contains the request data.
         * @param socket_FD The file descriptor for the accepted socket connection used when seding the HTTP response.
         *
         * @return Returns 0 on success, 1 for errors.
         *
         */
        int login_route(char *buffer, SOCKET socket_FD);

        /**
         *
         * @brief This function handles the '/addFile' route, sending an appropriate HTTP response to the client.
         *
         * @param buffer Contains the file data. This is used to retrive the metadata of the request such as the file name and file extension.
         * @param byteBuffer Contains the file data. This is used to write the data into an actual file on the local machine.
         * @param socket_FD The file descriptor for the accepted socket connection used when seding the HTTP response.
         * @param bytesReceived The size of the current buffer / byteBuffer
         *
         * @return Returns 0 on success, 1 for errors.
         *
         */
        int add_files_route(const char *buffer, const BYTE *binary_buffer, SOCKET socket_FD, unsigned int bytes_in);

        /**
         *
         * @brief This function handles the '/change_password' route, sending an appropriate HTTP response to the client.
         *
         * @param buffer Contains the request data.
         * @param socket_FD The file descriptor for the accepted socket connection used when seding the HTTP response.
         *
         * @return Returns 0 on success, 1 for errors.
         *
         */
        int change_password_route(char *buffer, SOCKET socket_FD);

        /**
         *
         * @brief This function handles the '/create_account' route, sending an appropriate HTTP response to the client.
         *
         * @param buffer Contains the request data.
         * @param socket_FD The file descriptor for the accepted socket connection used when seding the HTTP response.
         *
         * @return Returns 0 on success, 1 for errors.
         *
         */
        int create_account_route(char *buffer, SOCKET socket_FD);

        /**
         *
         * @brief This function handles the '/delete_files' route, sending an appropriate HTTP response to the client.
         *        It deletes the files from the Server. (automatically the files on the local machine will be deleted as well)
         *
         * @param buffer Contains the request data.
         * @param socket_FD The file descriptor for the accepted socket connection used when seding the HTTP response.
         *
         * @return Returns 0 on success, 1 for errors.
         *
         */
        int delete_file_route(char *buffer, SOCKET socket_FD);

        bool AUTH_STATUS; // This variable is used to check is the user is authentificated or not
        int  SESSION_ID;   // This variable is used to load the correct user data. It is asigned the ID of the user who logs into their account.

        std::vector<user_credentials>   uc;
        std::vector<user_files>         uf;

        std::string file_in_queue;

        MySQL_Handle m_mysql_handle;

        double TOTAL_BYTES_RECV;
    };

    int get_port(int argc, char **argv);
}