#include "database.hpp"

#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

#include <cstring>
#include <iomanip>

using namespace net::MySQL_DB;

int MySQL_Connector_Handler::init()
{
    char hostname[MAX_L], username[MAX_L], password[MAX_L];

    std::cout << "Hostname: ";
    std::cin.get(hostname, MAX_L);
    std::cin.get();

    std::cout << "\nUsername: ";
    std::cin.get(username, MAX_L);
    std::cin.get();

    std::cout << "\nPassword: ";
    std::cin.get(password, MAX_L);
    std::cin.get();

    std::cout << "\nSchema: ";
    std::cin.get(m_schema, MAX_L);

    try
    {
        m_driver = sql::mysql::get_mysql_driver_instance();

        m_conn = m_driver->connect("tcp://" + std::string(hostname), std::string(username), std::string(password));

        m_conn->setSchema(m_schema);
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

sql::ResultSet *MySQL_Connector_Handler::send_query(const std::string query) const noexcept
{
    try
    {
        sql::Statement *stmt = nullptr;
        sql::ResultSet *res  = nullptr;

        stmt = m_conn->createStatement();

        res = stmt->executeQuery(query);

        stmt->close();
        delete stmt;

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
    }

    return nullptr;
}

void MySQL_Connector_Handler::get_table_names() noexcept
{
    // sql::ResultSet *res = send_query("SELECT table_name FROM information_schema.tables WHERE table_schema='" 
    //                                     + std::string(m_schema) + "' AND table_type='BASE TABLE'");

    // while (res->next())
    //     m_data.emplace_back(res->getString("table_name").c_str());

    // res->close();
    // delete res;

#if DEBUG
    std::cout << "\nTables: ";
    for (unsigned int i = 0; i < m_tables.size(); i++)
        std::cout << m_tables[i] << std::endl;
    std::cout << "\n";
#endif
}

void MySQL_Connector_Handler::get_column_names() noexcept
{
    // for (unsigned int i = 0; i < 100; i++)
    // {
    //     sql::ResultSet *res = send_query("SELECT column_name FROM information_schema.columns WHERE table_schema = '" 
    //                                         + std::string(m_schema) + "' AND table_name = '" + std::string(m_data[i]->get_table_name()) + "'");

    //     std::cout << m_data[i]->get_table_name() << "\n";

    //     while (res->next())
    //         std::cout << res->getString("column_name") << std::endl;

    //     std::cout << "\n\n";
    // }
}