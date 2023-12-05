#include "database.hpp"

#include <mysql_connection.h>
#include <mysql_driver.h>

static bool getConnection(sql::Driver *driver, sql::Connection *con)
{

    driver = sql::mysql::get_mysql_driver_instance();

    con = driver->connect("tcp://", "", "");

    if (con == nullptr)
        return EXIT_FAILURE;

    con->setSchema("");

    return EXIT_SUCCESS;
}

int main()
{
    sql::Driver *driver = nullptr;
    sql::Connection *con = nullptr;

    if (!getConnection(driver, con))
        return EXIT_FAILURE;

    net::database *db = new net::database(driver, con, true);

    return EXIT_SUCCESS;
}