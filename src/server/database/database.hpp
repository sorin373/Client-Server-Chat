#pragma once
#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include <mysql_connection.h>
#include <mysql_driver.h>

namespace net
{
    class database
    {
    private:
        sql::Driver     *driver;
        sql::Connection *con;
        bool status;
    public:
        database(sql::Driver *driver, sql::Connection *con, bool status = false);

        sql::Driver *getDriver(void) const noexcept;
        sql::Connection *getCon(void) const noexcept;
        bool fetchTables(void);

        ~database() = default;
    };
};

#endif