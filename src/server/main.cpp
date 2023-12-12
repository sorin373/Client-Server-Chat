#include "serverUtils.hpp"
#include <mysql_connection.h>
#include <mysql_driver.h>

#include <iostream>

int main(int argc, char *argv[])
{
    sql::Driver *driver = nullptr;
    sql::Connection *con = nullptr;

    if (!net::server::__INIT__(argv[1]))
        return EXIT_FAILURE;

    return 0;
}