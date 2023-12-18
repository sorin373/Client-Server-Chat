#include "database.hpp"

#include <mysql_connection.h>
#include <mysql_driver.h>
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

std::vector<class server::database::SQLtable> server::database::getSqlTableVector(void) const noexcept
{
    return sqlTable;
}

void server::database::addSqlTable(char *tableName, size_t rowsCount)
{
    SQLtable st(tableName, rowsCount);
    sqlTable.push_back(st);
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
    std::cout << "hostname: ";
    std::cin.get(hostname, LENGHT);

    size_t len = strlen(hostname);

    if (len > HOSTNAME_LENGHT || len == 0)
        return EXIT_FAILURE;

    std::cin.get();
    std::cout << "username: ";
    std::cin.get(username, LENGHT);

    len = strlen(username);

    if (len > USERNAME_LENGHT || len == 0)
        return EXIT_FAILURE;

    std::cin.get();
    std::cout << "password: ";
    std::cin.get(password, LENGHT);

    len = strlen(password);

    if (len > PASSWORD_LENGHT || len == 0)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

server::database::dbCredentials::~dbCredentials()
{
    free(hostname);
    free(username);
    free(password);
}

/*SQLtable*/

server::database::SQLtable::SQLtable(char *tableName, const size_t rowsCount)
{
    this->tableName = tableName;
    this->rowsCount = rowsCount;
}

char *server::database::SQLtable::getTableName(void) const noexcept
{
    return tableName;
}

size_t server::database::SQLtable::getRowsCount(void) const noexcept
{
    return rowsCount;
}