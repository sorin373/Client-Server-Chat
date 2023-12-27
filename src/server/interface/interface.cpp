#include "interface.hpp"

#include "../serverUtils.hpp"
#include "../declarations.hpp"
#include "../database/database.hpp"

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <regex>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

using namespace net;
using namespace net::interface;

user::userCredentials::userCredentials(const char *username, const char *password, const int id)
{
    this->username = strdup(username);
    this->password = strdup(password);
    this->id = id;
}

char *user::userCredentials::getUsername(void) const noexcept
{
    return username;
}

char *user::userCredentials::getPassword(void) const noexcept
{
    return password;
}

int user::userCredentials::getId(void) const noexcept
{
    return id;
}

user::user()
{
    this->AUTH_STATUS = false;
    this->SESSION_ID = -1;
}

std::vector<class user::userCredentials> user::getUserCredentials(void) const noexcept
{
    return uc;
}

int user::getUserCredentialsSize(void) const noexcept
{
    return uc.size();
}

std::vector<class user::userFiles> user::getUserFiles(void) const noexcept
{
    return uf;
}

std::string user::getFileInQueue(void) const noexcept
{
    return fileInQueue;
}

int user::getSessionID(void) const noexcept
{
    return SESSION_ID;
}

bool user::getAuthStatus(void) const noexcept
{
    return AUTH_STATUS;
}

void user::resetAuthStatus(void) noexcept
{
    this->AUTH_STATUS = false;
}

void user::resetSessionID(void) noexcept
{
    this->SESSION_ID = -1;
}

void user::clearUserCredentials(void) noexcept
{
    uc.clear();
}

void user::clearUserFiles(void) noexcept
{
    uf.clear();
}

void user::clearFileInQueue(void) noexcept
{
    fileInQueue.clear();
}

void user::addToUserCredentials(const user::userCredentials __uc) noexcept
{
    uc.push_back(__uc);
}

void user::addToUserFiles(const user::userFiles __uf) noexcept
{
    uf.push_back(__uf);
}

void user::addFileInQueue(const std::string fileName) noexcept
{
    this->fileInQueue = fileName;
}

/* UserFiles table */

user::userFiles::userFiles(const char *fileName, const int uesrID, const int fileID, const int fileSize, const int noDownloads)
{
    this->fileName = strdup(fileName);
    this->userID = userID;
    this->fileID = fileID;
    this->fileSize = fileSize;
    this->noDownloads = noDownloads;
}

char *user::userFiles::getFileName(void) const noexcept
{
    return fileName;
}

int user::userFiles::getUserID(void) const noexcept
{
    return userID;
}

int user::userFiles::getFileID(void) const noexcept
{
    return fileID;
}

int user::userFiles::getFileSize(void) const noexcept
{
    return fileSize;
}

int user::userFiles::getNoDownloads(void) const noexcept
{
    return noDownloads;
}

void user::buildIndexHTML(void)
{
    std::ofstream index_html(INDEX_HTML);

    char firstHTML[] = R"(<!DOCTYPE html>
                        <html lang="en">

                        <head>
                            <meta charset="UTF-8"/>
                            <meta name="viewport" content="width=device-width, initial-scale=1.0"/>

                            <title>http-server</title>
                            
                            <link crossorigin="anonymous" href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css"
                                integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" rel="stylesheet"/>
                            <script crossorigin="anonymous" src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"
                                integrity="sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p"></script>

                            <link rel="icon" href="static/assets/logo.png"/>
                            <link href="static/stylesheet/index.css" rel="stylesheet"/>
                            <script src="static/javascript/deleteFileRequest.js"></script>
                            <script src="static/javascript/addFileRequest.js"></script>

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
                                    <p>http-server</p>
                                    <a class="btn btn-primary ext-ref" href="login.html">Logout</a>
                                </div>
                                <div class="container">
                                    <table class="table" style="margin: auto;">
                                        <thead>
                                            <tr>
                                                <th scope="col">#</th>
                                                <th scope="col">File Name</th>
                                                <th scope="col">File Size</th>
                                                <th scope="col">Downloads</th>
                                                <th scope="col" style="width: 85px;"></th>
                                            </tr>
                                        </thead>

                                        <tbody>)";

    index_html << firstHTML;

    char fileName[256] = "";
    char pdfExtension[] = ".pdf";

    for (unsigned int i = 0, n = uf.size(); i < n; i++)
    {
        strcpy(fileName, uf[i].getFileName());

        std::string th1 = "<tr><th scope=\"row\" class=\"left-column\">" + std::to_string(uf[i].getFileID()) + "</th>";
        std::string th2 = "<th scope=\"row\">" + std::string(uf[i].getFileName()) + "</th>";
        std::string th3 = "<th scope=\"row\">" + std::to_string(uf[i].getFileSize()) + "</th>";
        std::string th4 = "<th scope=\"row\">" + std::to_string(uf[i].getNoDownloads()) + "</th>";

        std::string td = R"(<td class="td-btn right-column">
                            <button type="button" class="delete-btn">
                                <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-trash" viewBox="0 0 16 16">
                                    <path
                                        d="M5.5 5.5A.5.5 0 0 1 6 6v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5m2.5 0a.5.5 0 0 1 .5.5v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5m3 .5a.5.5 0 0 0-1 0v6a.5.5 0 0 0 1 0z" />
                                    <path
                                        d="M14.5 3a1 1 0 0 1-1 1H13v9a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V4h-.5a1 1 0 0 1-1-1V2a1 1 0 0 1 1-1H6a1 1 0 0 1 1-1h2a1 1 0 0 1 1 1h3.5a1 1 0 0 1 1 1zM4.118 4 4 4.059V13a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1V4.059L11.882 4zM2.5 3h11V2h-11z" />
                                </svg>
                            </button>)";

        td = td + "<a href=\"storage/" + uf[i].getFileName() + "\" download=\"" + uf[i].getFileName() + "\">";

        td = td + R"(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-download" viewBox="0 0 16 16">
                                    <path d="M.5 9.9a.5.5 0 0 1 .5.5v2.5a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1v-2.5a.5.5 0 0 1 1 0v2.5a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2v-2.5a.5.5 0 0 1 .5-.5"/>
                                    <path d="M7.646 11.854a.5.5 0 0 0 .708 0l3-3a.5.5 0 0 0-.708-.708L8.5 10.293V1.5a.5.5 0 0 0-1 0v8.793L5.354 8.146a.5.5 0 1 0-.708.708l3 3z"/>
                                </svg>
                            </a></td></tr>)";

        std::string HTMLcontent = th1 + th2 + th3 + th4 + td;

        index_html << HTMLcontent;
    }

    char HTMLclose[] = R"(
        </tbody>
        </table>
        </div>

        <form action="/addFile" method="post" id="addFileForm" enctype="multipart/form-data"">
            <input type="file" name="filename">
            <input type="submit" class="btn btn-primary">
        </form>

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

bool user::validateCredentials(char *username, char *password)
{
    for (auto &__uc : uc)
        if (strcmp(username, __uc.getUsername()) == 0 && strcmp(password, __uc.getPassword()) == 0)
        {
            this->SESSION_ID = __uc.getId();
            return true;
        }

    return false;
}

bool user::findUsername(char *username)
{
    for (auto &__uc : uc)
        if (strcmp(username, __uc.getUsername()) == 0)
            return true;

    return false;
}

int user::loginRoute(char *buffer, int acceptedSocketFileDescriptor)
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

    if (strlen(temp_username) > NET_USERNAME_LENGHT || temp_username == nullptr)
        return EXIT_FAILURE;

    ptr = strtok(NULL, " ");

    if (ptr != NULL)
        for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
            if (ptr[i] == '=')
            {
                temp_password = &ptr[++i];
                break;
            }

    if (strlen(temp_password) > NET_PASSWORD_LENGHT || temp_password == nullptr)
        return EXIT_FAILURE;

    if (!validateCredentials(temp_username, temp_password))
    {
        if (send(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized), 0) == -1)
        {
            std::cerr << "Failed to send response.\n";
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    this->AUTH_STATUS = true;

    if (send(acceptedSocketFileDescriptor, authorized, strlen(authorized), 0) == -1)
    {
        std::cerr << "Failed to send response.\n";
        return EXIT_FAILURE;
    }

    __server->SQLfetchFileTable();

    buildIndexHTML();

    return EXIT_SUCCESS;
}

std::string fileName;
int fileCount;

int user::addFilesRoute(const char *buffer, const uint8_t *byteBuffer, int acceptedSocketFileDescriptor, ssize_t __bytesReceived)
{
    if (findString(buffer, "filename="))
    {
        fileName.clear();

        const std::string t_buffer = std::string(buffer);

        std::regex fileNameRegex(R"(filename=\"([^\"]+)\")");
        std::smatch match;

        if (std::regex_search(t_buffer, match, fileNameRegex))
            fileName = match.str(1);

        fileCount++;

        if (fileName.length() > 40)
        {
            std::string fileExtension;
            std::string newName;

            newName = "Upload" + std::to_string(fileCount);

            size_t pos = fileName.find(".");

            if (pos != std::string::npos)
            {
                fileExtension = fileName.substr(pos);
                newName = newName + fileExtension;
            }

            fileName = newName;
        }
        else
        {
            for (unsigned int i = 0, n = fileName.length(); i < n; i++)
                if (fileName[i] == ' ')
                    fileName[i] = '_';
        }
        
        addFileInQueue(fileName);
    }

    FILE *file = fopen("interface/storage/temp.bin", "ab");

    if (file != NULL)
    {
        fwrite(byteBuffer, sizeof(uint8_t), __bytesReceived, file);
        fclose(file);
    }
    else
    {
        std::cerr << "Failed to open file!\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int user::changePasswordRoute(char *buffer, int acceptedSocketFileDescriptor)
{
    char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /login.html\r\nConnection: close\r\n\r\n";
    char unauthorized[] = "HTTP/1.1 302 Found\r\nLocation: /changePassword.html\r\nConnection: close\r\n\r\n";

    char *username = nullptr, *oldPassword = nullptr, *newPassword = nullptr, *confirmation = nullptr, *ptr = nullptr;
    unsigned int count = 0;

    ptr = strstr(buffer, "username=");

    if (ptr != nullptr)
    {
        char *str = strtok(ptr, "&");

        while (str != NULL)
        {
            for (unsigned int i = 0, n = strlen(str); i < n; i++)
                if (str[i] == '=')
                    switch (count)
                    {
                    case 0:
                        username = &str[++i];
                        break;
                    case 1:
                        oldPassword = &str[++i];
                        break;
                    case 2:
                        newPassword = &str[++i];
                        break;
                    case 3:
                        confirmation = &str[++i];
                        break;

                    default:
                        break;
                    }

            str = strtok(NULL, "& ");
            count++;
        }

        if (strlen(username) > NET_USERNAME_LENGHT || username == nullptr)
            return EXIT_FAILURE;

        if (strlen(oldPassword) > NET_PASSWORD_LENGHT || oldPassword == nullptr)
            return EXIT_FAILURE;

        if (strlen(newPassword) > NET_PASSWORD_LENGHT || newPassword == nullptr)
            return EXIT_FAILURE;

        if (strlen(confirmation) > strlen(newPassword) || confirmation == nullptr)
            return EXIT_FAILURE;

        if (!validateCredentials(username, oldPassword) || strcmp(newPassword, confirmation) != 0)
        {
            if (send(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized), 0) == -1)
            {
                std::cerr << "Failed to send response.\n";
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }

        std::string query = "UPDATE user SET password=(?) WHERE username=(?)";
        sql::PreparedStatement *prepStmt = __server->getSQLdatabase()->getCon()->prepareStatement(query);

        prepStmt->setString(1, std::string(newPassword));
        prepStmt->setString(2, std::string(username));

        prepStmt->executeUpdate();

        delete prepStmt;

        __server->SQLfetchUserTable();

        if (send(acceptedSocketFileDescriptor, authorized, strlen(authorized), 0) == -1)
        {
            std::cerr << "Failed to send response.\n";
            return EXIT_FAILURE;
        }
    }

    if (send(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized), 0) == -1)
    {
        std::cerr << "Failed to send response.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int user::createAccountRoute(char *buffer, int acceptedSocketFileDescriptor)
{
    char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /login.html\r\nConnection: close\r\n\r\n";
    char unauthorized[] = "HTTP/1.1 302 Found\r\nLocation: /createAccount.html\r\nConnection: close\r\n\r\n";

    unsigned int count = 0;
    char *username = nullptr, *password = nullptr, *confirmation = nullptr, *ptr = nullptr;
    ptr = strstr(buffer, "username=");
    if (ptr != nullptr)
    {
        char *str = strtok(ptr, "&");

        while (str != NULL)
        {
            for (unsigned int i = 0, n = strlen(str); i < n; i++)
                if (str[i] == '=')
                    switch (count)
                    {
                    case 0:
                        username = &str[++i];
                        break;
                    case 1:
                        password = &str[++i];
                        break;
                    case 2:
                        confirmation = &str[++i];
                        break;

                    default:
                        break;
                    }

            str = strtok(NULL, "& ");
            count++;
        }

        if (strlen(username) > NET_USERNAME_LENGHT || username == nullptr)
            return EXIT_FAILURE;

        if (strlen(password) > NET_PASSWORD_LENGHT || password == nullptr)
            return EXIT_FAILURE;

        if (strlen(confirmation) > strlen(password) || confirmation == nullptr)
            return EXIT_FAILURE;

        if (findUsername(username) || strcmp(password, confirmation) != 0)
        {
            if (send(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized), 0) == -1)
            {
                std::cerr << "Failed to send response.\n";
                return EXIT_FAILURE;
            }

            return EXIT_SUCCESS;
        }
        
        std::string query = "INSERT INTO user VALUES (?, ?, ?)";
        sql::PreparedStatement *prepStmt = __server->getSQLdatabase()->getCon()->prepareStatement(query);

        prepStmt->setInt(1, uc.size());
        prepStmt->setString(2, std::string(username));
        prepStmt->setString(3, std::string(password));

        prepStmt->executeUpdate();

        delete prepStmt;

        __server->SQLfetchUserTable();

        if (send(acceptedSocketFileDescriptor, authorized, strlen(authorized), 0) == -1)
        {
            std::cerr << "Failed to send response.\n";
            return EXIT_FAILURE;
        }
    }

    if (send(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized), 0) == -1)
    {
        std::cerr << "Failed to send response.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int user::deleteFileRoute(char *buffer, int acceptedSocketFileDescriptor)
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

    std::string query = "DELETE FROM file WHERE file_id=(?)";
    sql::PreparedStatement *prepStmt = __server->getSQLdatabase()->getCon()->prepareStatement(query);

    prepStmt->setInt(1, fileID);

    prepStmt->executeUpdate();

    delete prepStmt;

    __server->SQLfetchFileTable();
    __server->getUser()->buildIndexHTML();

    if (send(acceptedSocketFileDescriptor, authorized, strlen(authorized), 0) == -1)
    {
        std::cerr << "Failed to send response.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}