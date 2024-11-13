#include "database.hpp"

#include <cppconn/prepared_statement.h>
#include <cstring>
#include <iomanip>

namespace ws_app::__detail
{
    int my_sql_handler::mysql_easy_init(const char *connection_name, const char *username, const char *password, const char *schema)
    {
        try
        {
            if (strlen(schema) > MAX_SCHEMA_L)
                throw std::invalid_argument("Too many characters!\n");

            strcpy(this->m_schema, schema);

            this->m_driver = sql::mysql::get_mysql_driver_instance();
            this->m_connection = m_driver->connect(connection_name, username, password);

            if (this->m_connection == nullptr)
                return -1;

            this->m_connection->setSchema(m_schema);
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "\n\n"
                    << std::setw(5) << " "
                    << "Error code: " << e.getErrorCode() << "\n"
                    << std::setw(5) << " "
                    << "Error message: " << e.what() << "\n"
                    << std::setw(5) << " "
                    << "SQLState: " << e.getSQLState() << "\n\n";

            return -1;
        }

        return EXIT_SUCCESS;
    }

    sql::ResultSet *my_sql_handler::send_query(const std::string &query)
    {
        sql::Statement *stmt = nullptr;
        sql::ResultSet *res = nullptr;

        try
        {
            stmt = this->m_connection->createStatement();
            res = stmt->executeQuery(query);

            return res;
        }
        catch (sql::SQLException &e)
        {
            std::cerr << "\n\n"
                    << std::setw(5) << " "
                    << "Error code: " << e.getErrorCode() << "\n"
                    << std::setw(5) << " "
                    << "Error message: " << e.what() << "\n"
                    << std::setw(5) << " "
                    << "SQLState: " << e.getSQLState() << "\n\n";

            if (stmt) delete stmt;
            if (res)  delete res;
        }

        return nullptr;
    }
}