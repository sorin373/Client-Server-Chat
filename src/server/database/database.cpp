// #include "../global.hpp"
// #include "database.hpp"

// #include <mysql_connection.h>
// #include <mysql_driver.h>
// #include <iomanip>
// #include <cstring>
// #include <vector>

// using namespace net;

// /* Database */

// template class server<char>;

// template <typename T>
// server<T>::database::database(sql::Driver *driver, sql::Connection *con,
//                            const char *hostname, const char *username, const char *password, const char *database)
// {
//     this->driver = driver;
//     this->con = con;
//     this->databaseCredentials = new dbCredentials(hostname, username, password, database);
// }

// template <typename T>
// sql::Connection *server<T>::database::getCon(void) const noexcept
// {
//     return con;
// }

// template <typename T>
// sql::Driver *server<T>::database::getDriver(void) const noexcept
// {
//     return driver;
// }

// template <typename T>
// class server<T>::database::dbCredentials *server<T>::database::getDbCredentials(void) const noexcept
// {
//     return databaseCredentials;
// }

// template <typename T>
// server<T>::database::~database()
// {
//     delete databaseCredentials;
//     con->close();
//     delete con;
// }

// /* dbCredentials */

// template <typename T>
// server<T>::database::dbCredentials::dbCredentials(const char *hostname, const char *username, const char *password, const char *database)
// {
//     this->hostname = strdup(hostname);
//     this->username = strdup(username);
//     this->password = strdup(password);
//     this->database = strdup(database);
// }

// template <typename T>
// char *server<T>::database::dbCredentials::getHostname(void) const noexcept
// {
//     return const_cast<char *>(hostname);
// }

// template <typename T>
// char *server<T>::database::dbCredentials::getUsername(void) const noexcept
// {
//     return const_cast<char *>(username);
// }

// template <typename T>
// char *server<T>::database::dbCredentials::getPassword(void) const noexcept
// {
//     return const_cast<char *>(password);
// }

// template <typename T>
// char *server<T>::database::dbCredentials::getDatabase(void) const noexcept
// {
//     return const_cast<char *>(database);
// }

// template <typename T>
// int server<T>::database::dbCredentials::getCredentials(char *hostname, char *username, char *password, char *database)
// {
//     system("clear");

//     std::cout << "\n\n"
//               << std::setw(13) << " "
//               << "DATABASE CONNECTION\n"
//               << std::setw(4) << " "
//               << "======================================\n"
//               << std::setw(5) << " "
//               << "Hostname: ";

//     std::cin.get(hostname, LENGHT);

//     size_t len = strlen(hostname);

//     if (len > HOSTNAME_LENGHT || len == 0)
//         return EXIT_FAILURE;

//     std::cin.get();
//     std::cout << std::setw(5) << " "
//               << "Username: ";
//     std::cin.get(username, LENGHT);

//     len = strlen(username);

//     if (len > USERNAME_LENGHT || len == 0)
//         return EXIT_FAILURE;

//     std::cin.get();
//     std::cout << std::setw(5) << " "
//               << "Password: ";

//     toggleEcho(false);
//     std::cin.get(password, LENGHT);
//     toggleEcho(true);

//     len = strlen(password);

//     if (len > PASSWORD_LENGHT || len == 0)
//         return EXIT_FAILURE;

//     std::cin.get();
//     std::cout << "\n" << std::setw(5) << " "
//               << "Database: ";
//     std::cin.get(database, LENGHT);

//     len = strlen(database);

//     if (len > DATABASE_LENGHT || len == 0)
//         return EXIT_FAILURE;

//     std::cout << std::setw(4) << " "
//               << "======================================\n";

//     return EXIT_SUCCESS;
// }

// template <typename T>
// server<T>::database::dbCredentials::~dbCredentials()
// {
//     free(this->hostname);
//     this->hostname = nullptr;

//     free(this->username);
//     this->username = nullptr;

//     free(this->password);
//     this->password = nullptr;

//     free(this->database);
//     this->database = nullptr;
// }