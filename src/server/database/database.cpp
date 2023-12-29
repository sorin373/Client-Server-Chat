#include "database.hpp"

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <iomanip>
#include <cstring>
#include <vector>

using namespace net;

/*Database*/

server::database::database(sql::Driver *driver, sql::Connection *con,
                           const char *hostname, const char *username, const char *password, const char *database)
{
    this->driver = driver;
    this->con = con;
    this->__credentials = new dbCredentials(hostname, username, password, database);
}

sql::Connection *server::database::getCon(void) const noexcept
{
    return con;
}

sql::Driver *server::database::getDriver(void) const noexcept
{
    return driver;
}

class server::database::dbCredentials *server::database::getDbCredentials(void) const noexcept
{
    return __credentials;
}

server::database::~database()
{
    delete __credentials;
    con->close();
    delete con;
}

/*Credentials*/

server::database::dbCredentials::dbCredentials(const char *hostname, const char *username, const char *password, const char *database)
{
    this->hostname = strdup(hostname);
    this->username = strdup(username);
    this->password = strdup(password);
    this->database = strdup(database);
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

char *server::database::dbCredentials::getDatabase(void) const noexcept
{
    return const_cast<char *>(database);
}

int server::database::dbCredentials::getCredentials(char *hostname, char *username, char *password, char *database)
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

    std::cin.get();
    std::cout << std::setw(5) << " "
              << "Database: ";
    std::cin.get(database, LENGHT);

    len = strlen(database);

    if (len > DATABASE_LENGHT || len == 0)
        return EXIT_FAILURE;

    std::cout << std::setw(4) << " "
              << "======================================\n";

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

    free(this->database);
    this->database = nullptr;
}