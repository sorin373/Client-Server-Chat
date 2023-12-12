#include "database.hpp"

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cstring>

using namespace net;

server::database::database(sql::Driver *driver, sql::Connection *con, const bool status)
{
    this->driver = driver;
    this->con = con;
    this->status = status;
}

sql::Connection *server::database::getCon(void) const noexcept
{
    return con;
}

sql::Driver *server::database::getDriver(void) const noexcept
{
    return driver;
}

bool server::database::getStatus(void) const noexcept
{
    return status;
}

server::database::credentials::credentials(const char hostname[], const char username[], const char password[])
{
    strcpy(this->hostname, hostname);
    strcpy(this->username, username);
    strcpy(this->password, password);
}

char *server::database::credentials::getHostname(void) const noexcept
{
    return const_cast<char *>(hostname);
}

char *server::database::credentials::getUsername(void) const noexcept
{
    return const_cast<char *>(username);
}

char *server::database::credentials::getPassword(void) const noexcept
{
    return const_cast<char *>(password);
}

bool server::database::credentials::getCredentials(void)
{
    char hostname[60], username[32], password[32];

    std::cout << "hostname: ";
    std::cin.get(hostname, 60);

    std::cin.get();
    std::cout << "username: ";
    std::cin.get(username, 32);

    std::cin.get();
    std::cout << "password: ";
    std::cin.get(password, 32);

    credentials(hostname, username, password);

    return EXIT_SUCCESS;
}

bool server::database::fetchTables(void)
{
    

    return EXIT_SUCCESS;
}
