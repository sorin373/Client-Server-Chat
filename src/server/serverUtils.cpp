#include "serverUtils.hpp"
#include "declarations.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string.h>
#include <sstream>
#include <vector>
#include <thread>
#include <netinet/in.h>
#include "interface/interface.hpp"
#include "database/database.hpp"
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

using namespace net;
using namespace net::interface;

volatile bool server::SERVER_RUNNING = false;

template void server::__SERVER_INIT__<char>(int serverSocketFileDescriptor);

server::server(const int clientSocketFileDescriptor)
{
    this->clientSocketFileDescriptor = clientSocketFileDescriptor;
    this->db = nullptr;
    this->__user = nullptr;
}

template <typename T>
void server::acceptConnection(const int serverSocketFileDescriptor, class acceptedSocket<T> *__acceptedSocket)
{
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);

    int clientSocketFD = accept(serverSocketFileDescriptor, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddressSize);

    __acceptedSocket->getAcceptedSocket(clientAddress, clientSocketFD, clientSocketFD > 0, clientSocketFD <= 0);
}

char *route = nullptr;
bool changeRoute = false;

template <typename T>
int server::POSTrequestsHandler(T *buffer, int acceptedSocketFileDescriptor, ssize_t __bytesReceived)
{
    uint8_t *byteBuffer = reinterpret_cast<uint8_t *>(buffer);
    char *charBuffer = reinterpret_cast<char *>(buffer);

    if (changeRoute) // determines the route from the first buffer
    {
        if (route != nullptr)
            delete[] route;

        char *ptr = new char[strlen(buffer) + 1];
        strcpy(ptr, buffer);

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
        if(__user->loginRoute(charBuffer, acceptedSocketFileDescriptor) == EXIT_FAILURE)
            return EXIT_FAILURE;

    if (findString(route, "/addFile"))
        if(__user->addFilesRoute(buffer, byteBuffer, acceptedSocketFileDescriptor, __bytesReceived) == EXIT_FAILURE)
            return EXIT_FAILURE;

    if (findString(route, "/change_password"))
        if (__user->changePasswordRoute(buffer, acceptedSocketFileDescriptor) == EXIT_FAILURE)
            return EXIT_FAILURE;

    if (findString(route, "/create_account"))
        if (__user->createAccountRoute(buffer, acceptedSocketFileDescriptor) == EXIT_FAILURE)
            return EXIT_FAILURE;

    if (findString(route, "/delete_file"))
        if (__user->deleteFileRoute(buffer, acceptedSocketFileDescriptor) == EXIT_FAILURE)
            return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

template <typename T>
int server::GETrequestsHandler(T *buffer, int acceptedSocketFileDescriptor)
{
    bool USE_DEFAULT_ROUTE = false;

    const char defaultRoute[] = "interface/login.html";
    const char root[] = "interface";
    char *path = nullptr;
    char *allocatedBuffer = reinterpret_cast<char *>(buffer);

    for (int i = 0, n = strlen(allocatedBuffer); i < n; i++)
        if (allocatedBuffer[i] == '/')
        {
            path = &allocatedBuffer[i];
            break;
        }

    if (path != nullptr)
        for (int i = 0, n = strlen(path); i < n; i++)
            if (path[i] == ' ')
                path[i] = '\0';

    if (path == nullptr || (strlen(path) == 1 && path[0] == '/'))
        USE_DEFAULT_ROUTE = true;

    if (strcmp(path, "/login.html") != 0 && strcmp(path, "/changePassword.html") != 0 && strcmp(path, "/createAccount.html") != 0  && 
        !findString(path, ".css") && !findString(path, ".png") && !__user->getAuthStatus())
            USE_DEFAULT_ROUTE = true;

    if (strcmp(path, "/login.html") == 0)
    {
        __user->resetAuthStatus();
        __user->resetSessionID();
    }
    
    char fullPath[strlen(root) + strlen(path) + 1] = "";
    
    if (path != nullptr && !findString(path, "interface"))
        strcpy(fullPath, root);

    strcat(fullPath, path);

    std::ifstream file;

    if (USE_DEFAULT_ROUTE)
        file.open(defaultRoute, std::ios::binary);
    else
        file.open(fullPath, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Failed to open: " << path << '\n';
        return EXIT_FAILURE;
    }

    std::ostringstream response;
    response << "HTTP/1.1 200 OK\r\nContent-Length: ";

    file.seekg(0, std::ios::end);
    int size = file.tellg();
    response << size << "\r\n\r\n";

    file.seekg(0, std::ios::beg);
    response << file.rdbuf();

    if (send(acceptedSocketFileDescriptor, response.str().c_str(), response.str().size(), 0) == -1)
    {
        std::cerr << "Failed to send response.\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

char *requestType = nullptr;

template <typename T>
int server::HTTPrequestsHandler(T *buffer, int acceptedSocketFileDescriptor, ssize_t __bytesReceived)
{
    char *charBuffer = reinterpret_cast<char *>(buffer);
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
        if (GETrequestsHandler<T>(buffer, acceptedSocketFileDescriptor) == EXIT_FAILURE)
        {
            delete[] copyBuffer;
            return EXIT_FAILURE;
        }

    if (strcmp(requestType, "POST") == 0)
        if (POSTrequestsHandler<T>(buffer, acceptedSocketFileDescriptor, __bytesReceived) == EXIT_FAILURE)
        {
            delete[] copyBuffer;
            return EXIT_FAILURE;
        }

    delete[] copyBuffer;

    return EXIT_SUCCESS;
}

int server::formatFile(const std::string fileName)
{
    std::ifstream file(BINARY_FILE_TEMP_PATH, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Failed to open: " << BINARY_FILE_TEMP_PATH << '\n';
        return EXIT_FAILURE;
    }

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
        if (!foundBoundary && line.find("------WebKitFormBoundary") != std::string::npos)
        {
            foundBoundary = true;
            continue;
        }

        if (foundBoundary && line.find("Content-Type:") != std::string::npos)
        {
            std::getline(file, line);

            while (std::getline(file, line))
            {
                if (line.find("------WebKitFormBoundary") != std::string::npos)
                    break;

                outFile << line << std::endl;
            }

            break;
        }
    }

    file.close();
    outFile.close();

    if (remove(BINARY_FILE_TEMP_PATH) != 0)
        std::cerr << "Failed to removed temp.bin!\n";

    return EXIT_SUCCESS;
}

void server::postRecv(const int acceptedSocketFileDescriptor)
{
    char response[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";
    std::string file = __user->getFileInQueue();

    if (!file.empty())
    {
        formatFile(file);

        addToFileTable(file.c_str(), 0);

        __user->clearFileInQueue();

        if (send(acceptedSocketFileDescriptor, response, strlen(response), 0) == -1)
            std::cerr << "Failed to send response.\n";
    }
}

template <typename T>
void server::receivedDataHandler(class acceptedSocket<T> *socket)
{
    int acceptedSocketFD = socket->getAcceptedSocketFileDescriptor();

    T buffer[1025];

    while (true)
    {
        ssize_t bytesReceived = recv(acceptedSocketFD, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0)
        {
            std::cerr << "-- Receive failed: "
                      << socket->getError() << "\n";

            postRecv(acceptedSocketFD);

            break;
        }

        if (DEBUG_FLAG)
            std::cout << "\n____________________________________________________________\n\n";

        buffer[bytesReceived] = '\0';

        if (DEBUG_FLAG)
            std::cout << buffer;

        HTTPrequestsHandler<T>(buffer, acceptedSocketFD, bytesReceived);
    }

    close(socket->getAcceptedSocketFileDescriptor());
    delete socket;
}

template <typename T>
void server::receivedDataHandlerThread(class acceptedSocket<T> *psocket)
{
    std::thread printThread(&server::receivedDataHandler<T>, this, psocket);
    printThread.detach();
}

template <typename T>
void server::handleClientConnections(int serverSocketFileDescriptor)
{
    while (server::SERVER_RUNNING)
    {
        server::acceptedSocket<T> *newAcceptedSocket = new server::acceptedSocket<T>();

        acceptConnection(serverSocketFileDescriptor, newAcceptedSocket);

        connectedSockets.push_back(*newAcceptedSocket);

        receivedDataHandlerThread<T>(newAcceptedSocket);
    }
}

void server::consoleListener(void)
{
    underline(75);

    char input[101] = "";

    while (SERVER_RUNNING)
    {
        std::cout << std::setw(5) << " " << "--> ";
        std::cin >> input;

        if (strcasecmp(input, "exit") == 0)
        {
            SERVER_RUNNING = false;

            if (!DEBUG_FLAG)
                system("clear");
        }            
    }
}

template <typename T>
void server::__SERVER_INIT__(int serverSocketFileDescriptor)
{
    SERVER_RUNNING = true;

    std::ofstream index;
    index.open(INDEX_HTML_PATH, std::ofstream::out | std::ofstream::trunc);
    index.close();

    std::thread workerThread(&server::handleClientConnections<T>, this, serverSocketFileDescriptor);
    workerThread.detach();

    std::thread consoleListenerThread(&consoleListener);
    consoleListenerThread.join();
}

int server::bindServer(int serverSocketFileDescriptor, struct sockaddr_in *serverAddress)
{
    return bind(serverSocketFileDescriptor, (struct sockaddr *)serverAddress, sizeof(struct sockaddr_in));
}

int server::getClientSocketFileDescriptor(void) const noexcept
{
    return clientSocketFileDescriptor;
}

template <typename S>
void server::acceptedSocket<S>::getAcceptedSocket(const struct sockaddr_in ipAddress, const int acceptedSocketFileDescriptor, const bool acceptStatus, const int error)
{
    this->ipAddress = ipAddress;
    this->acceptedSocketFileDescriptor = acceptedSocketFileDescriptor;
    this->acceptStatus = acceptStatus;
    this->error = error;
}

template <typename S>
struct sockaddr_in server::acceptedSocket<S>::getIpAddress(void) const noexcept
{
    return ipAddress;
}

template <typename S>
int server::acceptedSocket<S>::getError(void) const noexcept
{
    return error;
}

template <typename S>
bool server::acceptedSocket<S>::getAcceptStatus(void) const noexcept
{
    return acceptStatus;
}

template <typename S>
int server::acceptedSocket<S>::getAcceptedSocketFileDescriptor(void) const noexcept
{
    return acceptedSocketFileDescriptor;
}

template <typename T>
std::vector<class server::acceptedSocket<T>> server::getConnectedSockets(void) const noexcept
{
    return connectedSockets;
}

bool server::getServerStatus(void) const noexcept
{
    return SERVER_RUNNING;
}

int server::getNoOfConnectedSockets(void) const noexcept
{
    return connectedSockets.size();
}

class server::database *server::getSQLdatabase(void) const noexcept
{
    return db;
}

class interface::user *server::getUser(void) const noexcept
{
    return __user;
}

void server::SQLfetchUserTable(void)
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

void server::SQLfetchFileTable(void)
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

int server::addToFileTable(const char *fileName, const int fileSize)
{
    bool found = false;
    std::vector<class user::userFiles> __userFiles = __user->getUserFiles();

    for (const auto &__uf : __userFiles)
        if (strcmp(__uf.getFileName(), fileName) == 0)
        {
            found = true;
            break;
        }

    if (found)
        return EXIT_SUCCESS;

    try
    {
        std::string tableName = "file";
        std::string query = "INSERT INTO " + tableName + " (user_id, file_id, name, size, no_of_downloads) VALUES (?, ?, ?, ?, ?)";

        sql::PreparedStatement *prepStmt = db->getCon()->prepareStatement(query);

        prepStmt->setInt(1, __user->getSessionID());
        prepStmt->setInt(2, __userFiles.size());
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

int server::__database_init__(void)
{
    if (db != nullptr)
    {
        std::cerr << "Database is live!\n";
        return EXIT_SUCCESS;
    }

    char *hostname = (char *)malloc(LENGHT * sizeof(char) + 1);

    if (hostname == NULL)
    {
        std::cerr << "Failed to allocate hostname memory!\n";

        return EXIT_FAILURE;
    }

    char *username = (char *)malloc(LENGHT * sizeof(char) + 1);

    if (username == NULL)
    {
        std::cerr << "Failed to allocate username memory!\n";
        free(hostname);

        return EXIT_FAILURE;
    }

    char *password = (char *)malloc(LENGHT * sizeof(char) + 1);

    if (password == NULL)
    {
        std::cerr << "Failed to allocate password memory!\n";
        free(hostname);
        free(username);

        return EXIT_FAILURE;
    }

    if (server::database::dbCredentials::getCredentials(hostname, username, password) == EXIT_FAILURE)
    {
        std::cerr << "Failed to get MySQL schema credentails!\n";

        free(hostname);
        free(username);
        free(password);

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
            std::cerr << "Failed to establish a connection to the database.\n";

            free(hostname);
            free(username);
            free(password);

            return EXIT_FAILURE;
        }

        con->setSchema("Pinnacle");

        db = new server::database(driver, con, hostname, username, password);
        __user = new interface::user;
    }
    catch (sql::SQLException &e)
    {
        std::cerr << "\n"
                  << "Error code: " << e.getErrorCode() << "\n"
                  << "Error message: " << e.what() << "\n"
                  << "SQLState: " << e.getSQLState() << "\n";

        free(hostname);
        free(username);
        free(password);

        return EXIT_FAILURE;
    }

    free(hostname);
    free(username);
    free(password);

    SQLfetchUserTable();

    return EXIT_SUCCESS;
}

server::~server()
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

    connectedSockets.clear();
}