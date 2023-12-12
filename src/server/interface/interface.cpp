#include "interface.hpp"

#include <cstring>
#include <iostream>

using namespace net::interface;

user::userCredentials *user::uc = nullptr;

user::userCredentials::userCredentials(char *username, char *password, const int id)
{
    this->username = username;
    this->password = password;
    this->id = id;
}

void user::fetchCredentials(char *username, char *password, const int id)
{
    cleanup();
    uc = new userCredentials(username, password, id);
}

user::userCredentials *user::getUserCredentials(void) noexcept
{
    return uc;
}

void user::cleanup(void)
{
    if (uc)
    {
        delete uc;
        uc = nullptr;
    }
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

void net::interface::readFiles(void)
{
    /*

    TODO: creeate database to get the file names + other details

    */
}

static void findCredentials(char *__req)
{

}

bool net::interface::routeHandler(char *request) // request = username=test&password=test
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

    user::fetchCredentials(username, password, 0);

    // std::cout << user::getUserCredentials()->username << " " << user::getUserCredentials()->password << std::endl;

    return EXIT_SUCCESS;
}