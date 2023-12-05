#include "database.hpp"

#include <mysql_connection.h>
#include <mysql_driver.h>

net::database::database(sql::Driver *driver, sql::Connection *con, bool status)
{
    this->driver = driver;
    this->con = con;
    this->status = status;
}

sql::Connection *net::database::getCon(void) const noexcept
{
    return con;
}

sql::Driver *net::database::getDriver(void) const noexcept
{
    return driver;
}

bool net::database::fetchTables(void)
{
    return EXIT_SUCCESS;
}