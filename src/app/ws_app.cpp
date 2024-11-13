#include "ws_app.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <regex>
#include <cppconn/prepared_statement.h>

static bool isNumeric(const char *str)
{
    for (unsigned int i = 0, n = strlen(str); i < n; i++)
        if (!std::isdigit(str[i])) return false;

    return true;
}

namespace ws_app
{
    int get_port(int argc, char **argv)
    {
        int port = 0;

        if (argc > 2)
        {
            std::cerr << "\n"
                    << std::setw(5) << " "
                    << "--> Invalid number of arguments provided.\n\n"
                    << std::setw(5) << " "
                    << "--> Usage: \n"
                    << std::setw(5) << " "
                    << "       ./httpServer [port] [-debug]\n"
                    << std::setw(5) << " "
                    << "       ./httpServer [-debug]\n"
                    << std::setw(5) << " "
                    << "       ./httpServer [port]\n\n"
                    << std::setw(5) << " "
                    << "--> Optional flags:\n"
                    << std::setw(5) << " "
                    << "       [port]         Specify a particular port number for the Server to listen on.\n\n"
                    << std::setw(5) << " "
                    << "--> If no port is provided, the default port will be used.\n\n";

            return -1;
        }

        port = (port == 0) ? DEFAULT_PORT : port; 

        return port;
    }

    constexpr char INDEX_HTML_PATH[] =        "interface/index.html";
    constexpr char BINARY_FILE_TEMP_PATH[] =  "interface/storage/temp.bin";
    constexpr char LOCAL_STORAGE_PATH[] =     "interface/storage/";

    int core::app_easy_start()
    {
        system("clear");

        char connection_name[MAX_L], username[MAX_L], password[MAX_L], schema[MAX_L];
        
        std::cout << " =============================\n";
        std::cout << "       CONNECT DATABASE\n";
        std::cout << " =============================\n";

        std::cout << "  Hostname: ";
        std::cin.get(connection_name, MAX_L);
        std::cin.get();

        std::cout << "  Username: ";
        std::cin.get(username, MAX_L);
        std::cin.get();

        std::cout << "  Password: ";
        std::cin.get(password, MAX_L);
        std::cin.get();

        std::cout << "  Schema: ";
        std::cin.get(schema, MAX_L);
        std::cin.get();

        std::cout << " =============================\n";

        if (this->m_mysql_handle.mysql_easy_init(connection_name, username, password, schema) == -1)
            return -1;

        std::cout << "  --> The server successfully connected to the database (" << connection_name << ")\n";

        this->SQLfetch_file_table();
        this->SQLfetch_user_table();

        if (this->ws_easy_init() == -1)
            return -1;

        return 0;
    }

    HTTP_STATUS core::GET_request_handler(char *buffer, SOCKET client_socket_FD)
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
            else if (strcmp(path, "/login.html") != 0 && strcmp(path, "/changePassword.html") != 0 && strcmp(path, "/createAccount.html") != 0 &&
                    !strstr(path, ".css") && !strstr(path, ".png") && !this->AUTH_STATUS)
                USE_DEFAULT_ROUTE = true;

            if (!USE_DEFAULT_ROUTE)
            {
                if (strcmp(path, "/login.html") == 0)
                {
                    this->AUTH_STATUS = false;
                    this->SESSION_ID = -1;
                }

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

    void core::on_client_disconnected(SOCKET client_socket)
    {
        char response[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";
        std::string file = this->file_in_queue;

        if (!file.empty())
        {
            std::cout << std::setw(3) << " " << "| Formating file: '" << file << "'!\n";
            this->format_file(file);

            std::cout << std::setw(3) << " " << "| Adding '" << file << "' to the database!\n";

            if (this->add_to_file_table(file.c_str(), TOTAL_BYTES_RECV / 1000000.0000) == -1)
                std::cerr << std::setw(3) << " " << "|==DB== Failed to add '" << file << "' to the database!\n";

            std::cout << std::setw(3) << " " << "| Clearing '" << file << "' from queue!\n";
            this->file_in_queue.clear();

            TOTAL_BYTES_RECV = 0;

            std::cout << std::setw(3) << " " << "| Responding to client request!\n";

            this->send_to_client(client_socket, response, strlen(response) + 1);
        }
    }

    HTTP_STATUS core::route_manager(void *buffer, const char *route, SOCKET accepted_socket_FD, unsigned int bytes_in)
    {
        char *cbuff = reinterpret_cast<char*>(buffer);

        if (strstr(route, "/userlogin"))
        {
            if (this->login_route(cbuff, accepted_socket_FD) == EXIT_FAILURE)
                return HTTP_STATUS::SERVER_ERROR;
        }
        else if (strstr(route, "/addFile"))
        {
            // uint8_t used, because strict byte-level representation is needed
            BYTE *binary_buffer = reinterpret_cast<BYTE*>(buffer);

            if (this->add_files_route(cbuff, binary_buffer, accepted_socket_FD, bytes_in) == EXIT_FAILURE)
                return HTTP_STATUS::SERVER_ERROR;
        }
        else if (strstr(route, "/change_password"))
        {
            if (this->change_password_route(cbuff, accepted_socket_FD) == EXIT_FAILURE)
                return HTTP_STATUS::SERVER_ERROR;
        }
        else if (strstr(route, "/create_account"))
        {
            if (this->create_account_route(cbuff, accepted_socket_FD) == EXIT_FAILURE)
                return HTTP_STATUS::SERVER_ERROR;
        }
        else if (strstr(route, "/delete_file"))
        {
            if (this->delete_file_route(cbuff, accepted_socket_FD) == EXIT_FAILURE)
                return HTTP_STATUS::SERVER_ERROR;
        }

        return HTTP_STATUS::SUCCESS;
    }

    bool core::check_credentials(const char username[], const char password[])
    {
        for (user_credentials &t_uc : uc)
            if (strcmp(username, t_uc.username()) == 0 && strcmp(password, t_uc.password()) == 0)
            {
                this->SESSION_ID = t_uc.id();
                return true;
            }

        return false;
    }

    bool core::find_username(const char username[])
    {
        for (user_credentials &t_uc : uc)
            if (strcmp(username, t_uc.username()) == 0)
                return true;

        return false;
    }

    int core::format_file(const std::string &file_name)
    {
        // Open the binary file
        std::ifstream file(BINARY_FILE_TEMP_PATH, std::ios::binary);

        if (!file.is_open())
        {
            std::cerr << "==F== Failed to open (f_11): " << BINARY_FILE_TEMP_PATH << '\n';
            return EXIT_FAILURE;
        }

        // Open the new file that will contain the formatted file data
        std::ofstream outFile(std::string(LOCAL_STORAGE_PATH) + file_name, std::ios::binary);

        if (!outFile.is_open())
        {
            std::cerr << "==F== Failed to open (f_12): " << std::string(LOCAL_STORAGE_PATH) + file_name << '\n';
            file.close();

            return EXIT_FAILURE;
        }

        std::string line;
        bool foundBoundary = false;

        while (std::getline(file, line))
        {
            // Check for boundary
            if (!foundBoundary && line.find("------WebKitFormBoundary") != std::string::npos)
            {
                foundBoundary = true;
                continue;
            }

            if (foundBoundary && line.find("Content-Type:") != std::string::npos)
            {
                std::getline(file, line); // Skip line

                while (std::getline(file, line))
                {
                    // Read until the second boundary is found
                    if (line.find("------WebKitFormBoundary") != std::string::npos)
                        break;

                    outFile << line << std::endl;
                }

                break;
            }
        }

        file.close();
        outFile.close();

        // Remove 'temp.bin'
        if (remove(BINARY_FILE_TEMP_PATH) != 0)
            std::cerr << "==F== Failed to remove: '" << BINARY_FILE_TEMP_PATH << "'!\n";

        return EXIT_SUCCESS;
    }

    void core::build_index()
    {
        std::ofstream index_html(INDEX_HTML_PATH);

        if (!index_html.is_open())
        {
            std::cerr << std::setw(5) << " "
                    << "Error: Failed to open file: " << INDEX_HTML_PATH << "\n";
            return;
        }

        char firstHTML[] = R"(<!DOCTYPE html>
                            <html lang="en">

                            <head>
                                <meta charset="UTF-8"/>
                                <meta name="viewport" content="width=device-width, initial-scale=1.0"/>

                                <title>http-Server</title>
                                
                                <link crossorigin="anonymous" href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css"
                                    integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" rel="stylesheet"/>
                                <script crossorigin="anonymous" src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"
                                    integrity="sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p"></script>

                                <link rel="icon" href="static/assets/logo.png"/>
                                <link href="static/stylesheet/index.css" rel="stylesheet"/>
                                <script src="static/javascript/deleteFileRequest.js"></script>
                                <script src="static/javascript/addFileRequest.js"></script>
                                <script src="static/javascript/search.js"></script>

                                <style>
                                    a {
                                        text-decoration: none;
                                        font-size: 15pt;
                                        padding: 1px 6px;
                                        color: rgb(66, 66, 66) !important;
                                    }

                                    a:hover {
                                        color: #005ed8 !important;
                                    }
                                </style>
                            </head>

                            <body>
                                <div class="main-container">
                                    <div class="title-container">
                                        <input type="text" id="inputSearch" autofocus onkeyup="search() " placeholder="Search file">
                                        <a class="btn btn-primary ext-ref" href="login.html">Logout</a>
                                    </div>
                                    <div class="container">
                                        <table id="tableID" class="table" style="margin: auto;">
                                            <thead>
                                                <tr>
                                                    <th scope="col">#</th>
                                                    <th scope="col">File Name</th>
                                                    <th scope="col">File Size(mb)</th>
                                                    <th scope="col">Upload Date</th>
                                                    <th scope="col" style="width: 85px;"></th>
                                                </tr>
                                            </thead>

                                            <tbody>)";

        index_html << firstHTML;

        char file_name[256] = "";

        for (unsigned int i = 0, n = uf.size(); i < n; ++i)
        {
            strcpy(file_name, uf[i].file_name());

            std::string td1 = "<tr><td scope=\"row\" class=\"left-column\">" + std::to_string(uf[i].fileID()) + "</td>";
            std::string td2 = "<td scope=\"row\">" + std::string(uf[i].file_name()) + "</td>";
            std::string td3 = "<td scope=\"row\">" + std::to_string(uf[i].file_size()) + "</td>";
            std::string td4 = "<td scope=\"row\">" + std::string(uf[i].date()) + "</td>";

            std::string td = R"(<td class="td-btn right-column">
                                <button type="button" class="delete-btn">
                                    <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-trash" viewBox="0 0 16 16">
                                        <path
                                            d="M5.5 5.5A.5.5 0 0 1 6 6v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5m2.5 0a.5.5 0 0 1 .5.5v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5m3 .5a.5.5 0 0 0-1 0v6a.5.5 0 0 0 1 0z" />
                                        <path
                                            d="M14.5 3a1 1 0 0 1-1 1H13v9a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V4h-.5a1 1 0 0 1-1-1V2a1 1 0 0 1 1-1H6a1 1 0 0 1 1-1h2a1 1 0 0 1 1 1h3.5a1 1 0 0 1 1 1zM4.118 4 4 4.059V13a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1V4.059L11.882 4zM2.5 3h11V2h-11z" />
                                    </svg>
                                </button>)";

            td = td + "<a href=\"storage/" + uf[i].file_name() + "\" download=\"" + uf[i].file_name() + "\">";

            td = td + R"(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-download" viewBox="0 0 16 16">
                                        <path d="M.5 9.9a.5.5 0 0 1 .5.5v2.5a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1v-2.5a.5.5 0 0 1 1 0v2.5a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2v-2.5a.5.5 0 0 1 .5-.5"/>
                                        <path d="M7.646 11.854a.5.5 0 0 0 .708 0l3-3a.5.5 0 0 0-.708-.708L8.5 10.293V1.5a.5.5 0 0 0-1 0v8.793L5.354 8.146a.5.5 0 1 0-.708.708l3 3z"/>
                                    </svg>
                                </a></td></tr>)";

            std::string HTMLcontent = td1 + td2 + td3 + td4 + td;

            index_html << HTMLcontent;
        }

        char HTMLclose[] = R"(
            </tbody>
            </table>
            </div>

            <form action="/addFile" method="post" id="addFileForm" enctype="multipart/form-data">
            <label for="file-upload-btn" id="file-upload">Select a file</label>
            <input id="file-upload-btn" type="file" name="file_name" onchange="displayfile_name(this) ">
            <br>
            <label id="file-name-label">No file selected</label>
            <br>
            <input type="submit" class="btn btn-primary" value="Upload">
            </form>

            <progress id="uploadProgress" value="0" max="100"></progress>
            <div id="uploadStatus"></div>
            
            <script>
                function displayfile_name(input) {
                    var file_name = input.files[0].name;
                    var file_nameLabel = document.getElementById('file-name-label');
                    file_nameLabel.innerText = file_name;
                }
            </script>

            </div>

            <div class="footer">
                <p>Copyright (c) 2023 Sorin Tudose | </p>
                <a href="https://github.com/sorin373/HTTP-Server">GitHub</a>
            </div>

            </body>
            </html>
        )";

        index_html << HTMLclose;
        index_html.close();
    }

    void core::SQLfetch_user_table()
    {
        this->uc.clear();

        sql::ResultSet *res = this->m_mysql_handle.send_query("SELECT * FROM user");

        while (res->next())
        {
            int id = res->getInt("id");

            sql::SQLString sqlstr;

            sqlstr = res->getString("username");
            char *username = (char *)malloc(sqlstr.asStdString().length() + 1);
            strcpy(username, sqlstr.asStdString().c_str());

            sqlstr = res->getString("password");
            char *password = (char *)malloc(sqlstr.asStdString().length() + 1);
            strcpy(password, sqlstr.asStdString().c_str());

            // create an obj which we are pushing into the vector
            user_credentials t_uc(username, password, id);
            this->uc.push_back(t_uc);

            free(username);
            free(password);
        }

        res->close();
        delete res;
    }

    void core::SQLfetch_file_table()
    {
        this->uf.clear();

        sql::ResultSet *res = this->m_mysql_handle.send_query("SELECT * FROM file WHERE user_id=" + std::to_string(this->SESSION_ID));

        while (res->next())
        {
            int userID = res->getInt("user_id");
            int fileID = res->getInt("file_id");
            double fileSize = res->getDouble("size");
            std::string date = res->getString("date");

            sql::SQLString sqlstr;
            sqlstr = res->getString("name");
            char *file_name = (char *)malloc(sqlstr.asStdString().length() + 1);
            strcpy(file_name, sqlstr.asStdString().c_str());

            sqlstr = res->getString("date");
            char *t_date = (char *)malloc(sqlstr.asStdString().length() + 1);
            strcpy(t_date, sqlstr.asStdString().c_str());

            user_files t_uf(file_name, userID, fileID, fileSize, t_date);
            this->uf.push_back(t_uf);

            free(file_name);
            free(t_date);
        }

        res->close();
        delete res;
    }

    int core::add_to_file_table(const char *file_name, const double fileSize)
    {
        bool found = false;

        for (const user_files &t_uf : uf)
            if (strcmp(t_uf.file_name(), file_name) == 0)
            {
                found = true;
                break;
            }

        if (found)
            return EXIT_SUCCESS;

        int maxID = 0;

        sql::ResultSet *res = this->m_mysql_handle.send_query("SELECT file_id FROM file");

        while (res->next())
        {
            int fileID = res->getInt("file_id");

            if (fileID > maxID)
                maxID = fileID;
        }

        res->close();
        delete res;

        res = this->m_mysql_handle.send_query("SELECT sysdate() As date FROM dual");

        res->next();

        std::string date = res->getString("date");

        res->close();
        delete res;

        try
        {
            std::string query = "INSERT INTO file (user_id, file_id, name, size, date) VALUES (?, ?, ?, ?, ?)";

            sql::PreparedStatement *prepStmt = this->m_mysql_handle.connection()->prepareStatement(query);

            prepStmt->setInt(1, this->SESSION_ID);
            prepStmt->setInt(2, maxID + 1);
            prepStmt->setString(3, std::string(this->file_in_queue));
            prepStmt->setDouble(4, fileSize);
            prepStmt->setString(5, date);

            prepStmt->executeUpdate();

            prepStmt->close();
            delete prepStmt;
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "\n"
                    << "Error code: " << e.getErrorCode() << "\n"
                    << "Error message: " << e.what() << "\n"
                    << "SQLState: " << e.getSQLState() << "\n";

            return EXIT_FAILURE;
        }

        this->SQLfetch_file_table();
        this->build_index();

        return EXIT_SUCCESS;
    }

    /// Routes
    int core::login_route(char *buffer, SOCKET socket_FD)
    {
        char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";
        char unauthorized[] = "HTTP/1.1 302 Found\r\nLocation: /login.html\r\nConnection: close\r\n\r\n";

        char *temp_username, *temp_password, *ptr = NULL;

        ptr = strstr(buffer, "username=");
        ptr = strtok(ptr, "&");

        if (ptr != NULL)
            for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
                if (ptr[i] == '=')
                {
                    temp_username = &ptr[++i];
                    break;
                }

        if (temp_username == nullptr)
            return EXIT_FAILURE;

        if (strlen(temp_username) > MAX_USERNAME_LENGTH)
            return EXIT_FAILURE;

        ptr = strtok(NULL, " ");

        if (ptr != NULL)
            for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
                if (ptr[i] == '=')
                {
                    temp_password = &ptr[++i];
                    break;
                }

        if (temp_password == nullptr)
            return EXIT_FAILURE;

        if (strlen(temp_password) > MAX_PASSWORD_LENGTH)
            return EXIT_FAILURE;

        if (!this->check_credentials(temp_username, temp_password))
            send_to_client(socket_FD, unauthorized, strlen(unauthorized));

        this->AUTH_STATUS = true;

        send_to_client(socket_FD, authorized, strlen(authorized));

        SQLfetch_file_table();

        build_index();

        return EXIT_SUCCESS;
    }

    std::string file_name;
    int file_count;

    int core::add_files_route(const char *buffer, const BYTE *binary_buffer, SOCKET socket_FD, unsigned int bytes_in)
    {
        TOTAL_BYTES_RECV += bytes_in;

        if (strstr(buffer, "filename=") != nullptr)
        {
            file_name.clear();

            std::regex file_nameRegex(R"(filename=\"([^\"]+)\")");
            std::smatch match;

            const std::string t_buffer = std::string(buffer);

            // searches in the string 't_buffer' for matches
            if (std::regex_search(t_buffer, match, file_nameRegex))
                file_name = match.str(1);

            // Variable used to generate unique name for files which do not have a valid file name
            ++file_count;

            if (file_name.length() > 40)
            {
                std::string fileExtension;
                std::string newName;

                newName = "Upload" + std::to_string(file_count);

                size_t pos = file_name.find(".");

                if (pos != std::string::npos)
                {
                    fileExtension = file_name.substr(pos);
                    newName = newName + fileExtension;
                }

                file_name = newName;
            }
            else
            {
                // Replacing spaces with underlines
                for (unsigned int i = 0, n = file_name.length(); i < n; ++i)
                    if (file_name[i] == ' ')
                        file_name[i] = '_';
            }

            // Adds the new file in the queue to be inserted in the database
            this->file_in_queue = file_name;
        }

        FILE *file = fopen(BINARY_FILE_TEMP_PATH, "ab");

        if (file != NULL)
        {
            fwrite(binary_buffer, sizeof(uint8_t), bytes_in, file);
            fclose(file);
        }
        else
        {
            std::cerr << "Failed to open file: " << BINARY_FILE_TEMP_PATH << "\n";
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    int core::change_password_route(char *buffer, SOCKET socket_FD)
    {
        constexpr char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /login.html\r\nConnection: close\r\n\r\n";
        constexpr char unauthorized[] = "HTTP/1.1 302 Found\r\nLocation: /apology.html\r\nConnection: close\r\n\r\n";

        char *username = nullptr, *oldPassword = nullptr, *newPassword = nullptr, *confirmation = nullptr, *ptr = NULL;

        ptr = strstr(buffer, "username=");
        ptr = strtok(ptr, "&");

        if (ptr != NULL)
            for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
                if (ptr[i] == '=')
                {
                    username = &ptr[++i];
                    break;
                }

        if (username == nullptr)
            return EXIT_FAILURE;

        if (strlen(username) > MAX_USERNAME_LENGTH)
            return EXIT_FAILURE;

        ptr = strtok(NULL, "&");

        if (ptr != NULL)
            for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
                if (ptr[i] == '=')
                {
                    oldPassword = &ptr[++i];
                    break;
                }

        if (oldPassword == nullptr)
            return EXIT_FAILURE;

        if (strlen(oldPassword) > MAX_PASSWORD_LENGTH)
            return EXIT_FAILURE;

        ptr = strtok(NULL, "&");

        if (ptr != NULL)
            for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
                if (ptr[i] == '=')
                {
                    newPassword = &ptr[++i];
                    break;
                }

        if (newPassword == nullptr)
            return EXIT_FAILURE;

        if (strlen(newPassword) > MAX_PASSWORD_LENGTH)
            return EXIT_FAILURE;

        ptr = strtok(NULL, " ");

        if (ptr != NULL)
            for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
                if (ptr[i] == '=')
                {
                    confirmation = &ptr[++i];
                    break;
                }

        if (confirmation == nullptr)
            return EXIT_FAILURE;

        if (strlen(confirmation) > strlen(newPassword))
            return EXIT_FAILURE;

        // Check if old credentials are valid
        if (!this->check_credentials(username, oldPassword))
        {
            send_to_client(socket_FD, unauthorized, strlen(unauthorized) + 1);

            return EXIT_SUCCESS;
        }

        // Check if the new password is the same as the confirmation
        if (strcmp(newPassword, confirmation) != 0)
        {
            send_to_client(socket_FD, unauthorized, strlen(unauthorized) + 1);

            return EXIT_SUCCESS;
        }

        // Prepare query to update the user
        std::string query = "UPDATE user SET password=(?) WHERE username=(?)";
        sql::PreparedStatement *prepStmt = m_mysql_handle.connection()->prepareStatement(query);

        prepStmt->setString(1, std::string(newPassword));
        prepStmt->setString(2, std::string(username));

        // Execute the query
        prepStmt->executeUpdate();

        delete prepStmt;

        // Fetch the user table containg the updated data
        SQLfetch_user_table();

        send_to_client(socket_FD, authorized, strlen(authorized) + 1);

        return EXIT_SUCCESS;
    }

    int core::create_account_route(char *buffer, SOCKET socket_FD)
    {
        constexpr char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /login.html\r\nConnection: close\r\n\r\n";
        constexpr char unauthorized[] = "HTTP/1.1 302 Found\r\nLocation: /apology.html\r\nConnection: close\r\n\r\n";

        char *username = nullptr, *password = nullptr, *confirmation = nullptr, *ptr = NULL;

        ptr = strstr(buffer, "username=");
        ptr = strtok(ptr, "&");

        if (ptr != NULL)
            for (unsigned int i = 0, n = strlen(ptr); i < n; i++)
                if (ptr[i] == '=')
                {
                    username = &ptr[++i];
                    break;
                }

        if (username == nullptr)
            return EXIT_FAILURE;

        if (strlen(username) > MAX_USERNAME_LENGTH)
            return EXIT_FAILURE;

        ptr = strtok(NULL, "&");

        if (ptr != NULL)
            for (unsigned int i = 0, n = strlen(ptr); i < n; i++)
                if (ptr[i] == '=')
                {
                    password = &ptr[++i];
                    break;
                }

        if (password == nullptr)
            return EXIT_FAILURE;

        if (strlen(password) > MAX_PASSWORD_LENGTH)
            return EXIT_FAILURE;

        ptr = strtok(NULL, " ");

        if (ptr != NULL)
            for (unsigned int i = 0, n = strlen(ptr); i < n; i++)
                if (ptr[i] == '=')
                {
                    confirmation = &ptr[++i];
                    break;
                }

        if (confirmation == nullptr)
            return EXIT_FAILURE;

        if (strlen(confirmation) > strlen(password))
            return EXIT_FAILURE;

        // Check if the username already exists in the database
        if (this->find_username(username))
        {
            this->send_to_client(socket_FD, unauthorized, strlen(unauthorized) + 1);
            return EXIT_SUCCESS;
        }

        // Check if the password is the same as the confirmation
        if (strcmp(password, confirmation) != 0)
        {
            this->send_to_client(socket_FD, unauthorized, strlen(unauthorized) + 1);
            return EXIT_SUCCESS;
        }

        // Prepare query to insert the new account information
        std::string query = "INSERT INTO user VALUES (?, ?, ?)";
        sql::PreparedStatement *prepStmt = m_mysql_handle.connection()->prepareStatement(query);

        prepStmt->setInt(1, uc.size());
        prepStmt->setString(3, std::string(password));
        prepStmt->setString(2, std::string(username));

        // Execute the query
        prepStmt->executeUpdate();

        delete prepStmt;

        // Fetch the user table containg the updated data
        this->SQLfetch_user_table();

        this->send_to_client(socket_FD, authorized, strlen(authorized) + 1);

        return EXIT_SUCCESS;
    }

    int core::delete_file_route(char *buffer, SOCKET socket_FD)
    {
        char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";

        char *ptr = strstr(buffer, "fileID="), *cfileID = nullptr;
        int fileID = -1;

        if (ptr != nullptr)
            for (unsigned int i = 0, n = strlen(ptr); i < n; i++)
                if (ptr[i] == '=')
                {
                    cfileID = &ptr[++i];
                    break;
                }

        if (cfileID != nullptr)
            fileID = atoi(cfileID);

        // Fetch the file name using the file ID for later use
        std::string query = "SELECT name FROM file WHERE file_id=(?)";
        sql::ResultSet *res = nullptr;

        sql::PreparedStatement *prepStmt = m_mysql_handle.connection()->prepareStatement(query);

        prepStmt->setInt(1, fileID);
        res = prepStmt->executeQuery();

        while (res->next())
            std::string file_name = res->getString("name");

        delete res;
        delete prepStmt;

        // Delete the file from the database using the file ID
        std::string deleteQuery = "DELETE FROM file WHERE file_id=(?)";

        prepStmt = this->m_mysql_handle.connection()->prepareStatement(deleteQuery);

        prepStmt->setInt(1, fileID);
        prepStmt->executeUpdate();

        delete prepStmt;

        this->SQLfetch_file_table();
        this->build_index();

        // Using the file name remove the file from local storage
        std::string fileToDelete = std::string(LOCAL_STORAGE_PATH) + file_name;
        remove(fileToDelete.c_str());

        this->send_to_client(socket_FD, authorized, strlen(authorized) + 1);

        return EXIT_SUCCESS;
    }
}