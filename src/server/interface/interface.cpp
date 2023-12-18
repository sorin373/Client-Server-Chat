#include "interface.hpp"

#include "../serverUtils.hpp"
#include <cstring>
#include <iostream>
#include "../declarations.hpp"

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

user::userCredentials::~userCredentials() 
{
    //std::cout << "-------destroying...\n";

    // free(username);
    // free(password);
}

void user::resizeUserCredentialsVector(void) noexcept
{
    uc.resize(userCredentialsCount);
}

std::vector<class user::userCredentials> user::getUserCredentials(void) const noexcept
{
    return uc;
}

void user::addToUserCredentials(const user::userCredentials &__uc) noexcept
{
    uc.push_back(__uc);
}

// void writeHTMLhead(void)
// {  
//     char HTMLhead[] = R"(<head>
//     <meta charset="UTF-8" />
//     <meta name="viewport" content="width=device-width, initial-scale=1.0" />
//     <title>Document</title>
//     <link href="static/stylesheet/index.css" rel="stylesheet" />

//     <!-- http://getbootstrap.com/docs/5.1/ -->
//     <link crossorigin="anonymous" href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css"
//         integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" rel="stylesheet" />
//     <script crossorigin="anonymous" src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"
//         integrity="sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p"></script>
//     <link rel="icon" href="#" />
//     </head>)";
// }

void readFiles(void)
{
    /*

    TODO: creeate database to get the file names + other details

    */
}

static void findCredentials(char *__req)
{

}

bool user::validateCredentials(char *username, char *password) const
{
    for (unsigned int i = 0; i < uc.size(); i++)
        if (strcmp(username, uc[i].getUsername()) == 0 && strcmp(password, uc[i].getPassword()) == 0)
            return true;

    return false;
}

bool user::routeHandler(char *request, int acceptedSocketFileDescriptor) // request = username=test&password=test 
{
    char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";
    char unauthorized[] = "HTTP/1.1 401 Unauthorized\r\nContent-Length: 15\r\nConnection: close\r\n\r\nInvalid credentials";

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
        
    return EXIT_SUCCESS;
}