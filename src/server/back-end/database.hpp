#pragma once

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/resultset.h>
#include <cstring>

#define MAX_L 32
#define MAX_SCHEMA_L MAX_L

namespace net
{
    namespace __my_sql
    {
        class my_sql_handler
        {
        public:
             my_sql_handler() noexcept 
                : m_connection(nullptr), m_driver(nullptr), m_schema() { }

            explicit my_sql_handler(char *connection_name, char *username, char *password, char *schema)
                : m_connection(nullptr), m_driver(nullptr), m_schema()
            {
                if (this->init(connection_name, username, password, schema) == -1)
                    throw std::runtime_error("Failed to connect to the databse!\n");
            }

            int init(const char *connection_name, const char *username, const char *password, const char *schema);

            sql::ResultSet *send_query(const std::string &query);

            ~my_sql_handler() 
            { 
                if (this->m_connection != nullptr)
                {
                    this->m_connection->close();
                    delete this->m_connection;

                    this->m_connection = nullptr;
                }
            }

            sql::Driver* driver() const noexcept 
            { return this->m_driver; }

            sql::Connection* connection() const noexcept 
            { return this->m_connection; }

        private:
            sql::Connection           *m_connection;
            sql::Driver               *m_driver;
            char                       m_schema[MAX_SCHEMA_L];
        };
    }

    using MySQL_Handle = __my_sql::my_sql_handler;
}   