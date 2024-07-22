#ifndef __DATABASE_HPP__
#define __DATABASE_HPP__

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/resultset.h>
#include <cstring>
// #include "hash_table.hpp"

#define DEBUG 0

#define MAX_L 101

namespace net
{
    namespace MySQL_DB
    {
        class MySQL_Connector_Handler
        {
        public:
            MySQL_Connector_Handler() noexcept : m_conn(nullptr), m_driver(nullptr) { }

            int init();

            sql::ResultSet *send_query(const std::string query) const noexcept;

            ~MySQL_Connector_Handler() 
            { 
                m_conn->close();
                delete m_conn;
            }

            virtual void get_table_names() noexcept;

            virtual void get_column_names() noexcept;

            sql::Driver* get_driver() const noexcept { return m_driver; }

            sql::Connection* get_connection() const noexcept { return m_conn; }

            const char* get_schema() const noexcept { return m_schema; }

        private:
            // stl::hash_table<int, char> ht;
            sql::Connection           *m_conn;
            sql::Driver               *m_driver;
            char                       m_schema[MAX_L];
        };

        typedef MySQL_Connector_Handler sql_db;
    }

    typedef MySQL_DB::sql_db sql_db;
}

#endif