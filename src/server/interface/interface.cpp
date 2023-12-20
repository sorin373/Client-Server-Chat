#include "interface.hpp"

#include "../serverUtils.hpp"
#include "../declarations.hpp"

#include <iostream>
#include <fstream>
#include <cstring>

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

user::userCredentials::~userCredentials() {}

void user::resizeUserCredentialsVector(void) noexcept
{
    uc.resize(userCredentialsCount);
}

std::vector<class user::userCredentials> user::getUserCredentials(void) const noexcept
{
    return uc;
}

long long unsigned int user::getSessionID(void) const noexcept
{
    return SESSION_ID;
}

void user::addToUserCredentials(const user::userCredentials &__uc) noexcept
{
    uc.push_back(__uc);
}

void interface::buildIndexHTML(void)
{
    std::ifstream file(STORAGE_FILE_NAMES);
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
                              <link rel="icon" href="#" />

                              <style>
                                th {
                                    text-align: center;
                                }

                                a {
                                    text-decoration: none;
                                    font-size: 15pt;
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
                            </style>
                          </head>

                          <body>
                              <div class="container">
                                  <div>
                                      <table class="table" style="margin: auto; width: 100%; box-shadow: 0px 0px 10px rgba(0, 0, 0, 0.2);">
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

    while (file >> fileName)
    {
        char path[512] = "storage/";
        char link[256] = "<tr> <th scope=\"row\"> <a href=\"";

        strcat(path, fileName);
        strcat(path, pdfExtension);

        strcat(link, path);
        strcat(link, "\">");
        strcat(link, fileName);
        strcat(link, "</a> </th> </tr>");

        index_html << link;
    }

    char S1[] = "<button id=\"deleteBtn\"", S2[] = "onclick=\"deleteRequest()\"";

    strcat(S1, S2);

    char S3[] = R"(>Click me</button>
        </tbody>
        </table>
        </div>
        </div>
        </body>
        </html>
     )";

    strcat(S1, S3);

    index_html << S1;

    file.close();
    index_html.close();
}

bool user::validateCredentials(char *username, char *password) const
{
    for (unsigned int i = 0; i < uc.size(); i++)
        if (strcmp(username, uc[i].getUsername()) == 0 && strcmp(password, uc[i].getPassword()) == 0)
        {
            this->SESSION_ID = uc[i].getId();
            return true;
        }

    return false;
}

int user::routeHandler(char *request, int acceptedSocketFileDescriptor) // request = username=test&password=test
{
    char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";
    char unauthorized[] = "HTTP/1.1 401 Unauthorized\r\nContent-Length: 18\r\nConnection: close\r\n\r\nInvalid credentials";

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

    if (!user::validateCredentials(temp_username, temp_password))
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