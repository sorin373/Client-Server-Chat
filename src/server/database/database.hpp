#pragma once
#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include "../serverUtils.hpp"
#include <mysql_connection.h>
#include <mysql_driver.h>

namespace net
{
    class server::database
    {
    public:
        class credentials
        {
        private:
            char hostname[60], username[32], password[32];
        public:
            credentials(const char hostname[], const char username[], const char password[]);

            char *getHostname(void) const noexcept;
            char *getUsername(void) const noexcept;
            char *getPassword(void) const noexcept;
            static bool getCredentials(void);

            ~credentials() = default;
        };

    private:
        sql::Driver     *driver;
        sql::Connection *con;
        bool status;
       
    public:
        explicit database(sql::Driver *driver, sql::Connection *con, bool status = false);

        sql::Driver *getDriver(void)  const noexcept;
        sql::Connection *getCon(void) const noexcept;
        bool getStatus(void) const noexcept;
        bool fetchTables(void);

        ~database() = default;
    };
};

#endif