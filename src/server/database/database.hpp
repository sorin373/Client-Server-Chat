#pragma once
#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include "../serverUtils.hpp"
#include <mysql_connection.h>
#include <mysql_driver.h>

#define LENGHT          256
#define USERNAME_LENGHT 32
#define PASSWORD_LENGHT 32
#define HOSTNAME_LENGHT 61

namespace net
{
    class server::database
    {
    public:
        class credentials
        {
        private:
            char *hostname, *username, *password;
        public:
            credentials(const char *hostname, const char *username, const char *password);

            char *getHostname(void) const noexcept;
            char *getUsername(void) const noexcept;
            char *getPassword(void) const noexcept;
            static int getCredentials(char *hostname, char *username, char *password);

            ~credentials();
        };

    private:
        sql::Driver       *driver;
        sql::Connection   *con;
        class credentials *__credentials;
       
    public:
        explicit database(sql::Driver *driver, sql::Connection *con, const char *hostname, const char *username, const char *password );

        sql::Driver *getDriver(void)  const noexcept;
        sql::Connection *getCon(void) const noexcept;
        bool fetchTables(void);

        ~database();
    };
};

#endif