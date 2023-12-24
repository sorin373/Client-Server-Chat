#include "interface.hpp"

#include "../serverUtils.hpp"
#include "../declarations.hpp"

#include <iostream>
#include <fstream>
#include <cstring>
#include <malloc.h>

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
    this->SESSION_ID = 0;
}

std::vector<class user::userCredentials> user::getUserCredentials(void) const noexcept
{
    return uc;
}

std::vector<class user::userFiles> user::getUserFiles(void) const noexcept
{
    return uf;
}

int user::getSessionID(void) const noexcept
{
    return SESSION_ID;
}

void user::clearUserCredentials(void) noexcept
{
    uc.clear();
}

void user::clearUserFiles(void) noexcept
{
    uf.clear();
}

void user::addToUserCredentials(const user::userCredentials __uc) noexcept
{
    uc.push_back(__uc);
}

void user::addToUserFiles(const user::userFiles __uf) noexcept
{
    uf.push_back(__uf);
}

/* UserFiles table */

user::userFiles::userFiles(const char *fileName, const int id, const int fileSize, const int noDownloads)
{
    this->fileName = strdup(fileName);
    this->id = id;
    this->fileSize = fileSize;
    this->noDownloads = noDownloads;
}

char *user::userFiles::getFileName(void) const noexcept
{
    return fileName;
}

long long unsigned int user::userFiles::getId(void) const noexcept
{
    return id;
}

int user::userFiles::getFileSize(void) const noexcept
{
    return fileSize;
}

int user::userFiles::getNoDownloads(void) const noexcept
{
    return noDownloads;
}

void interface::user::buildIndexHTML(void)
{
    std::ofstream index_html(INDEX_HTML);

    char firstHTML[] = R"(<!DOCTYPE html>
                        <html lang="en">

                        <head>
                            <meta charset="UTF-8" />
                            <meta name="viewport" content="width=device-width, initial-scale=1.0" />
                            <title>Document</title>
                            <link href="static/stylesheet/index.css" rel="stylesheet" />

                            <link crossorigin="anonymous" href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css"
                                integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" rel="stylesheet" />
                            <script crossorigin="anonymous" src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"
                                integrity="sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p"></script>
                            <!-- Add icon library -->
                            <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css">

                            <link rel="icon" href="#" />

                            <style>
                                th {
                                    text-align: center;
                                }

                                a {
                                    text-decoration: none;
                                    font-size: 15pt;
                                    padding: 1px 6px;
                                }

                                thead {
                                    background-color: rgb(230, 230, 230);
                                    border-radius: 0;
                                    color: rgb(66, 66, 66);
                                }

                                table {
                                    width: 100%;
                                    max-width: 1400px;
                                }

                                thead {
                                    border: none !important;
                                    z-index: 1;
                                    position: sticky;
                                    top: 0;
                                }

                                thead th {
                                    font-size: 15pt;
                                    text-transform: uppercase;
                                    font-weight: 500;
                                    color: rgb(66, 66, 66);
                                    border: none !important;
                                }

                                #td-btn {
                                    display: flex;
                                    flex-direction: row;
                                }

                                button {
                                    border-style: none;
                                    background-color: transparent;
                                }

                                button:hover {
                                    background-color: rgb(235, 236, 236);
                                    border-radius: 5px;
                                }

                                tbody tr {
                                    color: rgb(66, 66, 66);
                                }

                                .left-column {
                                    border-right: solid 2px;
                                }

                                .right-column {
                                    border-left: solid 2px;
                                    width: fit-content;
                                }

                                .main-container {
                                    display: flex;
                                    flex-direction: column;
                                }

                                #add-button {
                                    width: fit-content;
                                    padding: 0.5rem;
                                    font-weight: 600;
                                    border: solid 0.1px rgb(153, 153, 153);
                                    border-radius: 5px;
                                    color: rgb(65, 64, 64);
                                }

                                #addFileForm {
                                    margin: auto;
                                    margin-top: 1rem;
                                }
                            </style>
                        </head>

                        <body>
                            <div class="main-container">
                                <div class="container">
                                    <table class="table" style="margin: auto; width: 100%;">
                                        <thead>
                                            <tr>
                                                <th scope="col">#</th>
                                                <th scope="col">File Name</th>
                                                <th scope="col">File Size</th>
                                                <th scope="col">Downloads</th>
                                                <th scope="col" style="width: 110px;"></th>
                                            </tr>
                                        </thead>

                                        <tbody>)";

    index_html << firstHTML;

    char fileName[256] = "";
    char pdfExtension[] = ".pdf";

    for (unsigned int i = 0, n = uf.size(); i < n; i++)
    {
        strcpy(fileName, uf[i].getFileName());

        std::string th1 = "<tr><th scope=\"row\" class=\"left-column\">" + std::to_string(uf[i].getId()) + "</th>";
        std::string th2 = "<th scope=\"row\">" + std::string(uf[i].getFileName()) + "</th>";
        std::string th3 = "<th scope=\"row\">" + std::to_string(uf[i].getFileSize()) + "</th>";
        std::string th4 = "<th scope=\"row\">" + std::to_string(uf[i].getNoDownloads()) + "</th>";

        std::string td = R"(<td class="td-btn right-column">
                            <button type="button">
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
                            </a>)";

        td = td + R"(<button type="button">
                                <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-share" viewBox="0 0 16 16">
                                    <path d="M13.5 1a1.5 1.5 0 1 0 0 3 1.5 1.5 0 0 0 0-3M11 2.5a2.5 2.5 0 1 1 .603 1.628l-6.718 3.12a2.499 2.499 0 0 1 0 1.504l6.718 3.12a2.5 2.5 0 1 1-.488.876l-6.718-3.12a2.5 2.5 0 1 1 0-3.256l6.718-3.12A2.5 2.5 0 0 1 11 2.5m-8.5 4a1.5 1.5 0 1 0 0 3 1.5 1.5 0 0 0 0-3m11 5.5a1.5 1.5 0 1 0 0 3 1.5 1.5 0 0 0 0-3"/>
                                </svg>
                            </button>
                            </td>
                            </tr>)";

        std::string HTMLcontent = th1 + th2 + th3 + th4 + td;

        index_html << HTMLcontent;
    }

    char HTMLclose[] = R"(
        </tbody>
        </table>
        </div>
        <form action="/addFile" method="post" id="addFileForm" enctype="multipart/form-data"">
            <input type="file" name="filename">
            <input type="submit">
        </form>
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

int user::loginRoute(char *request, int acceptedSocketFileDescriptor)
{
    char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";
    char unauthorized[] = "HTTP/1.1 401 Unauthorized\r\nContent-Length: 19\r\nConnection: close\r\n\r\nInvalid credentials";

    char *temp_username, *temp_password, *ptr = NULL;

    ptr = strstr(request, "username=");
    ptr = strtok(ptr, "&");

    if (ptr != NULL)
        for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
            if (ptr[i] == '=')
            {
                temp_username = &ptr[++i];
                break;
            }

    if (strlen(temp_username) > 32 || temp_username == nullptr)
        return EXIT_FAILURE;

    ptr = strtok(NULL, " ");

    if (ptr != NULL)
        for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
            if (ptr[i] == '=')
            {
                temp_password = &ptr[++i];
                break;
            }

    if (strlen(temp_password) > 64 || temp_password == nullptr)
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

    if (send(acceptedSocketFileDescriptor, authorized, strlen(authorized), 0) == -1)
    {
        std::cerr << "Failed to send response.\n";
        return EXIT_FAILURE;
    }

    __server->SQLfetchFileTable();

    buildIndexHTML();

    return EXIT_SUCCESS;
}

int user::addFilesRoute(char *request, int acceptedSocketFileDescriptor)
{   
    std::cout << request << "\n";

    return EXIT_SUCCESS;
}