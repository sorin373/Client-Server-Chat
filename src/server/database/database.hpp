/**
 *
 *  @file         database.hpp
 *
 *  @copyright    MIT License
 *
 *                Copyright (c) 2023 Sorin Tudose
 *
 *                Permission is hereby granted, free of charge, to any person obtaining a copy
 *                of this software and associated documentation files (the "Software"), to deal
 *                in the Software without restriction, including without limitation the rights
 *                to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *                copies of the Software, and to permit persons to whom the Software is
 *                furnished to do so, subject to the following conditions:
 *
 *                The above copyright notice and this permission notice shall be included in all
 *                copies or substantial portions of the Software.
 *
 *                THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *                IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *                FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *                AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *                LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *                OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *                SOFTWARE.
 *
 *  @brief        This C++ header file declares the nested 'database' class which contains core functionalites of a database
 *
 */

#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include "../serverUtils.hpp"

#include <mysql_connection.h>
#include <mysql_driver.h>

#define LENGHT          256
#define DATABASE_LENGHT 65
#define USERNAME_LENGHT 65
#define PASSWORD_LENGHT 65
#define HOSTNAME_LENGHT 65

namespace net
{
    // Class implementing core functionalities of database. However the tables are stored in the interface class.
    template <typename T> class server<T>::database
    {
    public:
        // Nested class describing the database credentials object.
        class dbCredentials
        {
        private:
            char *hostname, *username, *password, *database;
        public:
            dbCredentials(const char *hostname, const char *username, const char *password, const char *database);

            char *getHostname(void) const noexcept(true);
            char *getUsername(void) const noexcept(true);
            char *getPassword(void) const noexcept(true);
            char *getDatabase(void) const noexcept(true);

            /**
             * @brief This function retrieves the database credentials and checks if the input values are within the length limitation (This function does not validate the database credentails)
             * @return Returns 0 on success, 1 for errors.
             */
            static int getCredentials(char *hostname, char *username, char *password, char *database);

            ~dbCredentials();
        };

    private:
        class dbCredentials *databaseCredentials; // Pointer to the 'databaseCredentials' object.
        sql::Connection     *con;
        sql::Driver         *driver;
       
    public:
        database(sql::Driver *driver, sql::Connection *con, const char *hostname, const char *username, const char *password, const char *database);
        
        /**
         * @brief This function retrieves the SQL driver.
         * @return Returns a pointer to the SQL driver object.
         */
        sql::Driver *getDriver(void)  const noexcept(true);

        /**
         * @brief This function retrieves the database connection.
         * @return Returns a pointer to the SQL connection object.
         */
        sql::Connection *getCon(void) const noexcept(true);

        /**
         * @brief This function retrieves the database credentials.
         * @return Returns a pointer to the database credentials object.
         */
        class dbCredentials *getDbCredentials(void) const noexcept(true);

        ~database();
    };
};

#endif // __DATABASE_HPP__