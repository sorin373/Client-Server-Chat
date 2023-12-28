#include "database.hpp"

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <iomanip>
#include <cstring>
#include <vector>

using namespace net;

/*Database*/

server::database::database(sql::Driver *driver, sql::Connection *con,
                           const char *hostname, const char *username, const char *password)
{
    this->driver = driver;
    this->con = con;
    this->__credentials = new dbCredentials(hostname, username, password);
}

sql::Connection *server::database::getCon(void) const noexcept
{
    return con;
}

sql::Driver *server::database::getDriver(void) const noexcept
{
    return driver;
}

server::database::~database()
{
    delete __credentials;
    con->close();
    delete con;
}

/*Credentials*/

server::database::dbCredentials::dbCredentials(const char *hostname, const char *username, const char *password)
{
    this->hostname = strdup(hostname);
    this->username = strdup(username);
    this->password = strdup(password);
}

char *server::database::dbCredentials::getHostname(void) const noexcept
{
    return const_cast<char *>(hostname);
}

char *server::database::dbCredentials::getUsername(void) const noexcept
{
    return const_cast<char *>(username);
}

char *server::database::dbCredentials::getPassword(void) const noexcept
{
    return const_cast<char *>(password);
}

int server::database::dbCredentials::getCredentials(char *hostname, char *username, char *password)
{
    system("clear");

    std::cout << "\n\n"
              << std::setw(13) << " "
              << "DATABASE CONNECTION\n"
              << std::setw(4) << " "
              << "======================================\n"
              << std::setw(5) << " "
              << "Hostname: ";

    std::cin.get(hostname, LENGHT);

    size_t len = strlen(hostname);

    if (len > HOSTNAME_LENGHT || len == 0)
        return EXIT_FAILURE;

    std::cin.get();
    std::cout << std::setw(5) << " "
              << "Username: ";
    std::cin.get(username, LENGHT);

    len = strlen(username);

    if (len > USERNAME_LENGHT || len == 0)
        return EXIT_FAILURE;

    std::cin.get();
    std::cout << std::setw(5) << " "
              << "Password: ";
    std::cin.get(password, LENGHT);

    len = strlen(password);

    if (len > PASSWORD_LENGHT || len == 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

server::database::dbCredentials::~dbCredentials()
{
    free(this->hostname);
    this->hostname = nullptr;

    free(this->username);
    this->username = nullptr;

    free(this->password);
    this->password = nullptr;
}