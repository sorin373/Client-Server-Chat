#include "interface.hpp"

#include "../serverUtils.hpp"
#include <cstring>
#include <iostream>
#include "../declarations.hpp"

using namespace net;
using namespace net::interface;

void user::userCredentials::constructObject(char *username, char *password, const int id)
{    
    this->username = username;
    this->password = password;
    this->id = id;
}

void user::resizeUserCredentialsVector(void) noexcept
{
    uc.resize(userCredentialsCount);
}

std::vector<class user::userCredentials> user::getUserCredentials(void) const noexcept
{
    return uc;
}

void user::addToUserCredentials(char *username, char *password, const int id) noexcept
{
    user::userCredentials temp_uc;

    temp_uc.constructObject(username, password, id);

    uc.push_back(temp_uc);
}

void writeHTMLhead(void)
{  
    char HTMLhead[] = R"(<head>
    <meta charset="UTF-8" />
    <meta name="viewport" content="width=device-width, initial-scale=1.0" />
    <title>Document</title>
    <link href="static/stylesheet/index.css" rel="stylesheet" />

    <!-- http://getbootstrap.com/docs/5.1/ -->
    <link crossorigin="anonymous" href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css"
        integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" rel="stylesheet" />
    <script crossorigin="anonymous" src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"
        integrity="sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p"></script>
    <link rel="icon" href="#" />
    </head>)";
}

void readFiles(void)
{
    /*

    TODO: creeate database to get the file names + other details

    */
}

static void findCredentials(char *__req)
{

}

bool user::userCredentials::validateCredentials(char *username, char *password)
{

    return true;
}

// add user parameter + create user object for every thread
// than store the data into SQL table and clear the row (user credential) when logs out or 30 days passed.
bool user::routeHandler(char *request, int acceptedSocketFileDescriptor) // request = username=test&password=test 
{
    char *username, *password, *ptr = NULL;

    ptr = strstr(request, "username=");
    ptr = strtok(ptr, "&");

    if (ptr != NULL)
        for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
            if (ptr[i] == '=')
            {
                username = &ptr[++i];
                break;
            }

    if (strlen(username) > 32 || username == nullptr)
        return EXIT_FAILURE;

    ptr = strtok(NULL, " ");

    if (ptr != NULL)
        for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
            if (ptr[i] == '=')
            {
                password = &ptr[++i];
                break;
            }

    if (strlen(password) > 64 || password == nullptr)
        return EXIT_FAILURE;

    //std::cout << user::getUserCredentials()->username << " " << user::getUserCredentials()->password << std::endl;


    // TODO: add auth func to validat SQL table for correct username + password

    char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";
    char unauthorized[] = "HTTP/1.1 401 Unauthorized\r\nContent-Length: 15\r\nConnection: close\r\n\r\nInvalid credentials";

    if (send(acceptedSocketFileDescriptor, authorized, strlen(authorized), 0) == -1)
    {
        std::cerr << "Failed to send response.\n";
        return EXIT_FAILURE;
    }
        
    return EXIT_SUCCESS;
}