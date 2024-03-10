#include "global.hpp"
#include "interface/interface.hpp"

#ifndef serverUtils_hpp
#include "serverUtils.hpp"
#endif

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>
#include <thread>
#include <netinet/in.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

using namespace net;
using namespace net::interface;

template class server<char>;

template <typename T>
bool server<T>::SERVER_RUNNING = false;

/* db */

template <typename T>
server<T>::db::db(sql::Driver *driver, sql::Connection *con,
                  const char *hostname, const char *username, const char *password, const char *database)
{
    this->driver = driver;
    this->con = con;
    this->dbCred = new db_cred(hostname, username, password, database);
}

template <typename T>
sql::Connection *server<T>::db::getCon(void) const noexcept
{
    return con;
}

template <typename T>
sql::Driver *server<T>::db::getDriver(void) const noexcept
{
    return driver;
}

template <typename T>
class server<T>::db::db_cred *server<T>::db::getDB_Cred(void) const noexcept
{
    return dbCred;
}

template <typename T>
server<T>::db::~db()
{
    delete dbCred;
    con->close();
    delete con;
}

/* db_cred */

template <typename T>
server<T>::db::db_cred::db_cred(const char *hostname, const char *username, const char *password, const char *database)
{
    this->hostname = strdup(hostname);
    this->username = strdup(username);
    this->password = strdup(password);
    this->database = strdup(database);
}

template <typename T>
char *server<T>::db::db_cred::getHostname(void) const noexcept
{
    return const_cast<char *>(hostname);
}

template <typename T>
char *server<T>::db::db_cred::getUsername(void) const noexcept
{
    return const_cast<char *>(username);
}

template <typename T>
char *server<T>::db::db_cred::getPassword(void) const noexcept
{
    return const_cast<char *>(password);
}

template <typename T>
char *server<T>::db::db_cred::getDatabase(void) const noexcept
{
    return const_cast<char *>(database);
}

template <typename T>
int server<T>::db::db_cred::getCred(char *hostname, char *username, char *password, char *database)
{
    system("clear");

    std::cout << "\n\n"
              << std::setw(13) << " "
              << "DATABASE CONNECTION\n"
              << std::setw(4) << " "
              << "======================================\n"
              << std::setw(5) << " "
              << "Hostname: ";

    std::cin.get(hostname, LENGHT);

    size_t len = strlen(hostname);

    if (len > SQL_LENGHT || len == 0)
        return EXIT_FAILURE;

    std::cin.get();
    std::cout << std::setw(5) << " "
              << "Username: ";
    std::cin.get(username, LENGHT);

    len = strlen(username);

    if (len > SQL_LENGHT || len == 0)
        return EXIT_FAILURE;

    std::cin.get();
    std::cout << std::setw(5) << " "
              << "Password: ";

    toggleEcho(false);
    std::cin.get(password, LENGHT);
    toggleEcho(true);

    len = strlen(password);

    if (len > SQL_LENGHT || len == 0)
        return EXIT_FAILURE;

    std::cin.get();
    std::cout << "\n"
              << std::setw(5) << " "
              << "Database: ";
    std::cin.get(database, LENGHT);

    len = strlen(database);

    if (len > SQL_LENGHT || len == 0)
        return EXIT_FAILURE;

    std::cout << std::setw(4) << " "
              << "======================================\n";

    return EXIT_SUCCESS;
}

template <typename T>
server<T>::db::db_cred::~db_cred()
{
    free(this->hostname);
    this->hostname = nullptr;

    free(this->username);
    this->username = nullptr;

    free(this->password);
    this->password = nullptr;

    free(this->database);
    this->database = nullptr;
}

/* server */

template <typename T>
server<T>::server()
{
    this->db = nullptr;
    this->__user = nullptr;
}

template <typename T>
void server<T>::acceptConnection(const int serverSocketFD, class acceptedSocket &__acceptedSocket)
{
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);

    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddressSize);

    __acceptedSocket.getAcceptedSocket(clientAddress, clientSocketFD, clientSocketFD > 0);
}

char *route = nullptr;
bool changeRoute = false;

template <typename T>
int server<T>::POSTrequestsHandler(T *__buffer, int acceptedSocketFD, ssize_t __bytesReceived)
{
    uint8_t *byteBuffer = reinterpret_cast<uint8_t *>(__buffer);
    char *charBuffer = reinterpret_cast<char *>(__buffer);

    // Upon getting the initial buffer, it establishes the pathway for incoming data until 'changeRoute' is reset again.
    if (changeRoute)
    {
        if (route != nullptr)
            delete[] route;

        char *ptr = new char[strlen(charBuffer) + 1];
        strcpy(ptr, charBuffer);

        for (unsigned int i = 0, n = strlen(ptr); i < n; i++)
            if (ptr[i] == '/')
            {
                route = new char[strlen(ptr + i) + 1];
                strcpy(route, ptr + i);

                break;
            }

        for (unsigned int i = 0, n = strlen(route); i < n; i++)
            if (route[i] == ' ')
                route[i] = '\0';

        delete[] ptr;

        changeRoute = false;
    }

    if (findString(route, "/userlogin"))
        if (!__user->loginRoute(charBuffer, acceptedSocketFD))
            return EXIT_FAILURE;

    if (findString(route, "/addFile"))
        if (!__user->addFilesRoute(__buffer, byteBuffer, acceptedSocketFD, __bytesReceived))
            return EXIT_FAILURE;

    if (findString(route, "/change_password"))
        if (!__user->changePasswordRoute(__buffer, acceptedSocketFD))
            return EXIT_FAILURE;

    if (findString(route, "/create_account"))
        if (!__user->createAccountRoute(__buffer, acceptedSocketFD))
            return EXIT_FAILURE;

    if (findString(route, "/delete_file"))
        if (!__user->deleteFileRoute(__buffer, acceptedSocketFD))
            return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

template <typename T>
int server<T>::GETrequestsHandler(T *__buffer, int acceptedSocketFD)
{
    bool USE_DEFAULT_ROUTE = false;

    const char defaultRoute[] = "interface/login.html";
    const char root[] = "interface";
    char *path = nullptr;
    char *allocatedBuffer = reinterpret_cast<char *>(__buffer);

    if (allocatedBuffer == nullptr)
        return EXIT_FAILURE;

    for (int i = 0, n = strlen(allocatedBuffer); i < n; i++)
        if (allocatedBuffer[i] == '/')
        {
            path = &allocatedBuffer[i];
            break;
        }

    if (path == nullptr)
        USE_DEFAULT_ROUTE = true;

    std::ifstream file;

    if (!USE_DEFAULT_ROUTE)
    {
        for (int i = 0, n = strlen(path); i < n; i++)
            if (path[i] == ' ')
                path[i] = '\0';

        if ((strlen(path) == 1 && path[0] == '/'))
            USE_DEFAULT_ROUTE = true;

        if (strcmp(path, "/apology.html") != 0 && strcmp(path, "/login.html") != 0 && strcmp(path, "/changePassword.html") != 0 &&
            strcmp(path, "/createAccount.html") != 0 && !findString(path, ".css") && !findString(path, ".png") && !__user->getAuthStatus())
            USE_DEFAULT_ROUTE = true;

        if (!USE_DEFAULT_ROUTE)
        {
            if (strcmp(path, "/login.html") == 0)
            {
                __user->resetAuthStatus();
                __user->resetSessionID();
            }

            char fullPath[strlen(root) + strlen(path) + 1] = "";

            if (!findString(path, "interface"))
                strcpy(fullPath, root);

            strcat(fullPath, path);

            file.open(fullPath, std::ios::binary);
        }
    }

    if (USE_DEFAULT_ROUTE)
        file.open(defaultRoute, std::ios::binary);

    if (!file.is_open())
    {
        if (DEBUG_FLAG)
            std::cerr << std::setw(5) << " "
                      << "--> Encountered an error while attempting to open the file: " << path << '\n';
        return EXIT_FAILURE;
    }

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\nContent-Length: ";

    file.seekg(0, std::ios::end);
    int size = file.tellg();
    response << size << "\r\n\r\n";

    file.seekg(0, std::ios::beg);
    response << file.rdbuf();

    if (send(acceptedSocketFD, response.str().c_str(), response.str().size(), 0) == -1)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Failed to send HTTP response.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

char *requestType = nullptr;

template <typename T>
int server<T>::HTTPrequestsHandler(T *__buffer, int acceptedSocketFD, ssize_t __bytesReceived)
{
    char *charBuffer = reinterpret_cast<char *>(__buffer);
    char *copyBuffer = new char[strlen(charBuffer) + 1];

    strcpy(copyBuffer, charBuffer);

    char *ptr = strstr(copyBuffer, "GET");

    if (ptr == NULL)
    {
        strcpy(copyBuffer, charBuffer);
        ptr = strstr(copyBuffer, "POST");
    }

    if (ptr != NULL)
    {
        if (requestType != nullptr)
            delete[] requestType;

        changeRoute = true;

        for (unsigned int i = 0, n = strlen(ptr); i < n; i++)
            if (ptr[i] == ' ')
                ptr[i] = '\0';

        requestType = new char[strlen(ptr) + 1];

        strcpy(requestType, ptr);
    }

    if (strcmp(requestType, "GET") == 0)
        if (GETrequestsHandler(__buffer, acceptedSocketFD) == EXIT_FAILURE)
        {
            delete[] copyBuffer;
            return EXIT_FAILURE;
        }

    if (strcmp(requestType, "POST") == 0)
        if (POSTrequestsHandler(__buffer, acceptedSocketFD, __bytesReceived) == EXIT_FAILURE)
        {
            delete[] copyBuffer;
            return EXIT_FAILURE;
        }

    delete[] copyBuffer;

    return EXIT_SUCCESS;
}

template <typename T>
int server<T>::formatFile(const std::string fileName)
{
    // Open the create binary file
    std::ifstream file(BINARY_FILE_TEMP_PATH, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Failed to open: " << BINARY_FILE_TEMP_PATH << '\n';
        return EXIT_FAILURE;
    }

    // Open the new file that will contain the formatted file data
    std::ofstream outFile(std::string(LOCAL_STORAGE_PATH) + fileName, std::ios::binary);

    if (!outFile.is_open())
    {
        std::cerr << "Failed to open: " << std::string(LOCAL_STORAGE_PATH) + fileName << '\n';
        file.close();

        return EXIT_FAILURE;
    }

    std::string line;
    bool foundBoundary = false;

    while (std::getline(file, line))
    {
        // Check for boundary
        if (!foundBoundary && line.find("------WebKitFormBoundary") != std::string::npos)
        {
            foundBoundary = true;
            continue;
        }

        if (foundBoundary && line.find("Content-Type:") != std::string::npos)
        {
            std::getline(file, line); // Skip line

            while (std::getline(file, line))
            {
                // Read until the second boundary is found
                if (line.find("------WebKitFormBoundary") != std::string::npos)
                    break;

                outFile << line << "\n";
            }

            break;
        }
    }

    file.close();
    outFile.close();

    // Remove 'temp.bin'
    if (remove(BINARY_FILE_TEMP_PATH) != 0)
        std::cerr << "Failed to removed temp.bin!\n";

    return EXIT_SUCCESS;
}

template <typename T>
void server<T>::postRecv(const int acceptedSocketFD)
{     
    char response[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";

    std::string file = __user->getFileInQueue();

    if (!file.empty())
    {
        formatFile(file);

        TOTAL_BYTES_RECV /= 1024;

        addToFileTable(file.c_str(), TOTAL_BYTES_RECV);

        TOTAL_BYTES_RECV = 0;

        __user->clearFileInQueue();

        if (send(acceptedSocketFD, response, strlen(response), 0) == -1)
            std::cerr << std::setw(5) << " "
                      << "--> Error: Failed to send HTTP response.\n";
    }
}

template <typename T>
void server<T>::receivedDataHandler(const class acceptedSocket __socket)
{
    int acceptedSocketFD = __socket.getAcceptedSocketFD();

    T buffer[1025];

    while (true)
    {
        ssize_t bytesReceived = recv(acceptedSocketFD, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0)
        {
            if (DEBUG_FLAG)
                std::cerr << "\n"
                          << std::setw(5) << " "
                          << "--> Receive failed: "
                          << __socket.getError() << "\n";

            postRecv(acceptedSocketFD);

            break;
        }

        if (DEBUG_FLAG)
        {
            std::cout << "\n";
            underline(75);
        }

        buffer[bytesReceived] = '\0';

        if (DEBUG_FLAG)
            std::cout << buffer;

        HTTPrequestsHandler(buffer, acceptedSocketFD, bytesReceived);
    }

    if (acceptedSocketFD >= 0)
        close(acceptedSocketFD);
}

template <typename T>
void server<T>::receivedDataHandlerThread(const class acceptedSocket __socket)
{
    std::thread(&server::receivedDataHandler, this, __socket).detach();
}

template <typename T>
void server<T>::handleClientConnections(int serverSocketFD)
{
    while (SERVER_RUNNING == true)
    {
        acceptedSocket newAcceptedSocket;

        acceptConnection(serverSocketFD, newAcceptedSocket);

        //connectedSockets.push_back(newAcceptedSocket);

        receivedDataHandlerThread(newAcceptedSocket);
    }
}

template <typename T>
void server<T>::consoleListener(void)
{
    underline(75);

    char input[101] = "";

    while (SERVER_RUNNING)
    {
        std::cout << std::setw(5) << " "
                  << "--> ";
        std::cin >> input;

        if (strcasecmp(input, "exit") == 0)
        {
            std::cout << std::setw(5) << " " 
                      << "Shutting down...\n";

            sleep(1);

            SERVER_RUNNING = false;

            if (!DEBUG_FLAG)
                system("clear");

            break;
        }
    }
}

template <typename T>
void server<T>::server_easy_init(int serverSocketFD)
{
    SERVER_RUNNING = true;

    std::thread(&server::handleClientConnections, this, serverSocketFD).detach();

    consoleListener();
}

template <typename T>
int server<T>::bindServer(int serverSocketFD, struct sockaddr_in *__serverAddress)
{
    return bind(serverSocketFD, (struct sockaddr *)__serverAddress, sizeof(struct sockaddr_in));
}

template <typename T>
std::vector<class server<T>::acceptedSocket> server<T>::getConnectedSockets(void) const noexcept
{
    return connectedSockets;
}

template <typename T>
bool server<T>::getServerStatus(void) const noexcept
{
    return SERVER_RUNNING;
}

template <typename T>
void server<T>::haltServer(void) noexcept
{
    this->SERVER_RUNNING = false;
}

template <typename T>
class server<T>::db *server<T>::getSQLdatabase(void) const noexcept
{
    return db;
}

template <typename T>
class interface::user *server<T>::getUser(void) const noexcept
{
    return __user;
}

template <typename T>
void server<T>::SQLfetchUserTable(void)
{
    __user->clearUserCredentials();

    sql::Statement *stmt = nullptr;
    sql::ResultSet *res = nullptr;

    stmt = db->getCon()->createStatement();
    res = stmt->executeQuery("SELECT * FROM user");

    while (res->next())
    {
        int id = res->getInt("id");

        sql::SQLString sqlstr;

        sqlstr = res->getString("username");
        char *username = (char *)malloc(sqlstr.asStdString().length() + 1);
        strcpy(username, sqlstr.asStdString().c_str());

        sqlstr = res->getString("password");
        char *password = (char *)malloc(sqlstr.asStdString().length() + 1);
        strcpy(password, sqlstr.asStdString().c_str());

        user::userCredentials t_uc(username, password, id); // create an obj which we are pushing into the vector

        __user->addToUserCredentials(t_uc);

        free(username);
        free(password);
    }

    res->close();
    stmt->close();

    delete res;
    delete stmt;
}

template <typename T>
void server<T>::SQLfetchFileTable(void)
{
    __user->clearUserFiles();

    sql::Statement *stmt = nullptr;
    sql::ResultSet *res = nullptr;

    std::string SQLquery = "SELECT * FROM file WHERE user_id=" + std::to_string(__user->getSessionID());

    stmt = db->getCon()->createStatement();
    res = stmt->executeQuery(SQLquery);

    while (res->next())
    {
        int userID = res->getInt("user_id");
        int fileID = res->getInt("file_id");
        int fileSize = res->getInt("size");
        int downloads = res->getInt("no_of_downloads");

        sql::SQLString sqlstr;
        sqlstr = res->getString("name");
        char *fileName = (char *)malloc(sqlstr.asStdString().length() + 1);
        strcpy(fileName, sqlstr.asStdString().c_str());

        user::userFiles t_uf(fileName, userID, fileID, fileSize, downloads);

        __user->addToUserFiles(t_uf);

        free(fileName);
    }

    res->close();
    stmt->close();

    delete res;
    delete stmt;
}

template <typename T>
int server<T>::addToFileTable(const char *fileName, const int fileSize)
{
    std::vector<class user::userFiles> __userFiles = __user->getUserFiles();

    for (const auto &__uf : __userFiles)
        if (strcmp(__uf.getFileName(), fileName) == 0)
            return EXIT_SUCCESS;

    int maxID = 0;

    sql::Statement *stmt = nullptr;
    sql::ResultSet *res = nullptr;

    stmt = db->getCon()->createStatement();
    res = stmt->executeQuery("SELECT file_id FROM file");

    while (res->next())
    {
        int fileID = res->getInt("file_id");
        if (fileID > maxID)
            maxID = fileID;
    }

    delete stmt;
    delete res;

    try
    {
        std::string tableName = "file";
        std::string query = "INSERT INTO " + tableName + " (user_id, file_id, name, size, no_of_downloads) VALUES (?, ?, ?, ?, ?)";

        sql::PreparedStatement *prepStmt = db->getCon()->prepareStatement(query);

        prepStmt->setInt(1, __user->getSessionID());
        prepStmt->setInt(2, maxID + 1);
        prepStmt->setString(3, std::string(fileName));
        prepStmt->setInt(4, fileSize);
        prepStmt->setInt(5, 0);

        prepStmt->executeUpdate();

        delete prepStmt;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "\n"
                  << "Error code: " << e.getErrorCode() << "\n"
                  << "Error message: " << e.what() << "\n"
                  << "SQLState: " << e.getSQLState() << "\n";

        return EXIT_FAILURE;
    }

    SQLfetchFileTable();

    __user->buildIndexHTML();

    return EXIT_SUCCESS;
}

template <typename T>
int server<T>::database_easy_init(void)
{
    if (db != nullptr)
    {
        std::cout << std::setw(5) << " "
                  << "--> Database is already running.\n";
        return EXIT_SUCCESS;
    }

    char *hostname = (char *)malloc(LENGHT * sizeof(char) + 1);

    if (hostname == NULL)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Failed to allocate hostname memory.\n";

        return EXIT_FAILURE;
    }

    char *username = (char *)malloc(LENGHT * sizeof(char) + 1);

    if (username == NULL)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Failed to allocate username memory.\n";
        free(hostname);

        return EXIT_FAILURE;
    }

    char *password = (char *)malloc(LENGHT * sizeof(char) + 1);

    if (password == NULL)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Failed to allocate password memory.\n";
        free(hostname);
        free(username);

        return EXIT_FAILURE;
    }

    char *database = (char *)malloc(LENGHT * sizeof(char) + 1);

    if (database == NULL)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Failed to allocate database name memory.\n";
        free(hostname);
        free(username);
        free(password);

        return EXIT_FAILURE;
    }

    if (server<T>::db::db_cred::getCred(hostname, username, password, database) == EXIT_FAILURE)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Failed to fetch MySQL schema credentails.\n";

        free(hostname);
        free(username);
        free(password);
        free(database);

        return EXIT_FAILURE;
    }

    try
    {
        sql::Driver *driver = nullptr;
        sql::Connection *con = nullptr;

        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect("tcp://" + std::string(hostname), std::string(username), std::string(password));

        if (con == nullptr)
        {
            std::cerr << std::setw(5) << " "
                      << "--> Error: Failed to establish a connection to the database.\n";

            free(hostname);
            free(username);
            free(password);
            free(database);

            return EXIT_FAILURE;
        }

        con->setSchema(database);

        db = new typename server<T>::db(driver, con, hostname, username, password, database);
        __user = new interface::user;
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

        free(hostname);
        free(username);
        free(password);
        free(database);

        return EXIT_FAILURE;
    }

    free(hostname);
    free(username);
    free(password);
    free(database);

    SQLfetchUserTable();

    return EXIT_SUCCESS;
}

int net::INIT(int argc, char *argv[])
{
    std::cout << "Starting...\n";

    int port = 0;

    //port = getMainArguments(argc, argv);

    // if (port == -1)
    //     return EXIT_FAILURE;

    SocketUtils serverSocket;

    int serverSocketFD = serverSocket.createSocket();

    if (serverSocketFD == -1)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Socket creation failed.\n";

        return EXIT_FAILURE;
    }

    __server = new server<char>();

    if (__server->database_easy_init() == EXIT_FAILURE)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Database initialization failed.\n";

        shutdown(serverSocketFD, SHUT_RDWR);
        serverSocket.closeSocket(serverSocketFD);

        delete __server;
        
        return EXIT_FAILURE;
    }

    char *machineIPv4Address = serverSocket.getMachineIPv4Address();

    if (machineIPv4Address == nullptr)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Null pointer encountered for machine's IPv4 address.\n";

        shutdown(serverSocketFD, SHUT_RDWR);
        serverSocket.closeSocket(serverSocketFD);
        
        delete __server;

        return EXIT_FAILURE;
    }

    struct sockaddr_in *serverAddress = serverSocket.IPv4Address(machineIPv4Address, 8080);

    if (__server->bindServer(serverSocketFD, serverAddress) != 0)
    {
        std::cerr << "\n"
                  << std::setw(5) << " "
                  << "--> Server encountered an error during the binding process.\n"
                  << std::setw(5) << " "
                  << "--> ";

        perror("Bind");

        std::cout << "\n";

        shutdown(serverSocketFD, SHUT_RDWR);
        serverSocket.closeSocket(serverSocketFD);

        free(serverAddress);
        delete __server;
        delete[] machineIPv4Address;

        return EXIT_FAILURE;
    }

    system("clear");

    std::cout << "\n\n"
              << std::setw(5) << " "
              << "+-------------+\n"
              << std::setw(5) << " "
              << "| HTTP-SERVER |\n"
              << std::setw(5) << " "
              << "+-------------+\n";

    underline(75);

    std::cout << std::setw(5) << " "
              << "--> Database authentication successful!\n"
              << std::setw(5) << " "
              << "--> Server socket bound successfully!\n"
              << std::setw(5) << " "
              << "--> Access the server through your web browser via: \e[1m" << machineIPv4Address << ":" << port << "\e[0m\n"
              << std::setw(5) << " "
              << "    In order to shutdown the server type 'exit' in the console!\n";

    if (listen(serverSocketFD, 10) == -1)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Failed to initiate listening on the server socket.\n";

        shutdown(serverSocketFD, SHUT_RDWR);
        serverSocket.closeSocket(serverSocketFD);

        free(serverAddress);
        delete __server;
        delete[] machineIPv4Address;

        return EXIT_FAILURE;
    }

    __server->server_easy_init(serverSocketFD);

    sleep(5);

    shutdown(serverSocketFD, SHUT_RDWR);
    serverSocket.closeSocket(serverSocketFD);

    free(serverAddress);
    delete __server;
    delete[] machineIPv4Address;

    return EXIT_SUCCESS;
}

template <typename T>
server<T>::~server()
{
    if (this->db != nullptr)
    {
        delete this->db;
        this->db = nullptr;
    }

    if (this->__user != nullptr)
    {
        delete this->__user;
        this->__user = nullptr;
    }

    for (struct acceptedSocket &accSock : connectedSockets)
    {
        int socketFD = accSock.getAcceptedSocketFD();
        if (socketFD >= 0) 
            close(socketFD);
    }
    
    connectedSockets.clear();
}

/* acceptedSocket */

template <typename T>
void server<T>::acceptedSocket::getAcceptedSocket(const struct sockaddr_in ipAddress, const int acceptedSocketFD, const int error)
{
    this->ipAddress = ipAddress;
    this->acceptedSocketFD = acceptedSocketFD;
    this->error = error;
}

template <typename T>
struct sockaddr_in server<T>::acceptedSocket::getIpAddress(void) const noexcept
{
    return ipAddress;
}

template <typename T>
int server<T>::acceptedSocket::getError(void) const noexcept
{
    return error;
}

template <typename T>
int server<T>::acceptedSocket::getAcceptedSocketFD(void) const noexcept
{
    return acceptedSocketFD;
}