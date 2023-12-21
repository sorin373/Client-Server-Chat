#include "serverUtils.hpp"

#include <iostream>
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
#include "declarations.hpp"

using namespace net;
using namespace net::interface;

volatile bool server::SERVER_RUNNING = false;

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

bool findString(const char haystack[], const char needle[]) // created this bec. strstr modifies the string
{
    char *__copyHaystack = new char[strlen(haystack) + 1];
    strcpy(__copyHaystack, haystack);

    if (strstr(__copyHaystack, needle) != NULL)
    {
        delete[] __copyHaystack;
        return true;
    }
    
    delete[] __copyHaystack;
    return false;
}

template <typename T>
int server::handleGETrequests(T *buffer, int acceptedSocketFileDescriptor)
{
    const char root[] = "interface";
    char *path = nullptr;
    char *allocatedBuffer = buffer;
    char *copyBuffer = new char[strlen(buffer) + 1];
        
    strcpy(copyBuffer, allocatedBuffer);

    for (int i = 0, n = strlen(allocatedBuffer); i < n; i++)
        if (allocatedBuffer[i] == '/')
        {
            path = &allocatedBuffer[i];
            break;
        }

    for (int i = 0, n = strlen(path); i < n; i++)
        if (path[i] == ' ')
            path[i] = '\0';

    if ((strlen(path) == 1 && path[0] == '/') || path == nullptr)
        strcpy(path, "interface/login.html");

    if (findString(allocatedBuffer, "/userlogin") == true)
    {
        __user->loginRoute(copyBuffer, acceptedSocketFileDescriptor);
        delete[] copyBuffer;
        
        return EXIT_SUCCESS;
    }

    delete[] copyBuffer;
    
    char fullPath[strlen(root) + strlen(path) + 1] = "";

    if (findString(path, "interface") == false)
        strcpy(fullPath, root);

    strcat(fullPath, path);

    std::cout << "FULL PATH: " << fullPath << "\n\n";

    std::ifstream file(fullPath, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Failed to open " << path << "!\n";
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

template <typename T>
void server::sendReceivedMessage(T *buffer, int acceptedSocketFileDescriptor)
{
    for (auto &socket : connectedSockets)
    {
        int socketFD = socket.getAcceptedSocketFileDescriptor();

        if (socketFD != acceptedSocketFileDescriptor)
            send(socketFD, buffer, strlen(buffer), 0);
    }
}

template <typename T>
void server::printReceivedData(class acceptedSocket<T> *socket)
{
    T buffer[1025];

    while (true)
    {
        int acceptedSocketFD = socket->getAcceptedSocketFileDescriptor();
        ssize_t bytesReceived = recv(acceptedSocketFD, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0)
        {
            std::cerr << "Receive failed! "
                      << socket->getError() << "\n";
            break;
        }

        std::cout << "===============================================================================================================================\n\n";

        buffer[bytesReceived] = '\0';
        std::cout << buffer;

        handleGETrequests(buffer, acceptedSocketFD);

        // net::server::sendReceivedMessage(buffer, acceptedSocketFD);
    }

    close(socket->getAcceptedSocketFileDescriptor());
    delete socket;
}

template <typename T>
void server::printReceivedDataThread(class acceptedSocket<T> *psocket)
{
    std::thread printThread(&server::printReceivedData<char>, this, psocket);
    printThread.detach();
}

void server::handleClientConnections(int serverSocketFileDescriptor)
{
    while (server::SERVER_RUNNING)
    {
        server::acceptedSocket<char> *newAcceptedSocket = new server::acceptedSocket<char>();

        acceptConnection(serverSocketFileDescriptor, newAcceptedSocket);

        connectedSockets.push_back(*newAcceptedSocket);

        printReceivedDataThread(newAcceptedSocket);
    }

    close(serverSocketFileDescriptor);
}

void server::consoleListener(void)
{
    char input[101] = "";

    while (SERVER_RUNNING)
    {
        std::cin >> input;

        if (strcasecmp(input, "exit") == 0)
            SERVER_RUNNING = false;
    }
}

void server::__MASTER_THREAD__(int serverSocketFileDescriptor)
{
    SERVER_RUNNING = true;

    std::thread workerThread(&server::handleClientConnections, this, serverSocketFileDescriptor);
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
server::acceptedSocket<S>::acceptedSocket() {}

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

int server::getTableRowsCount(const char tableName[]) // returns the no. of rows which I use to resize the vector correctly
{
    if (__server->getSQLdatabase() == nullptr || __server->getSQLdatabase()->getSqlTableVector().empty())
        return 0;

    std::vector<class server::database::SQLtable> _SQLtable = __server->getSQLdatabase()->getSqlTableVector();

    if (_SQLtable.empty())
        return 0;

    for (auto &sqltable : _SQLtable)
        if (strcmp(tableName, sqltable.getTableName()) == 0)
            return sqltable.getRowsCount();
    
    return -1;
}

void server::SQLfetchUserTable(void)
{
    __user->clearUserCredentials();

    sql::Statement *stmt = nullptr;
    sql::ResultSet *res = nullptr;

    stmt = db->getCon()->createStatement();
    res = stmt->executeQuery("SELECT * FROM user");

    db->addSqlTable(tableName, res->rowsCount());

    __user->resizeUserCredentialsVector();

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

        user::userCredentials *t_uc = new user::userCredentials(username, password, id); // create an obj which we are pushing into the vector

        __user->addToUserCredentials(*t_uc);

        delete t_uc; // free the created obj

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

    std::string SQLquery = "SELECT * FROM file WHERE id=" + std::to_string(__user->getSessionID());

    stmt = db->getCon()->createStatement();
    res = stmt->executeQuery(SQLquery);

    db->addSqlTable(tableName, res->rowsCount());

    while (res->next())
    {
        int id = res->getInt("id");
        int fileSize = res->getInt("size");
        int downloads = res->getInt("no_of_downloads");

        sql::SQLString sqlstr;
        sqlstr = res->getString("name");
        char *fileName = (char *)malloc(sqlstr.asStdString().length() + 1);
        strcpy(fileName, sqlstr.asStdString().c_str());

        user::userFiles *t_uf = new user::userFiles(fileName, id, fileSize, downloads);

        __user->addToUserFiles(*t_uf);

        delete t_uf;

        free(fileName);
    }

    res->close();
    stmt->close();

    delete res;
    delete stmt;
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
        sql::Driver     *driver = nullptr;
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

    SQLfetchUserTable(); // get all users

    return EXIT_SUCCESS;
}

server::~server()
{
    delete db;
    delete __user;
}

int server::__INIT__(char *portArg)
{
    int port = 0;

    if (portArg == nullptr)
        port = DEFAULT_PORT;
    else
        port = atoi(portArg);

    SocketUtils *serverSocket = new SocketUtils;

    int serverSocketFD = serverSocket->createSocket();

    if (serverSocketFD == -1)
    {
        std::cerr << "Failed to create socket!\n";
        delete serverSocket;

        return EXIT_FAILURE;
    }

    __server = new server(serverSocketFD);
    
    userCredentialsCount = __server->getTableRowsCount(tableName);

    if (__server->__database_init__() == EXIT_FAILURE)
    {
        shutdown(serverSocketFD, SHUT_RDWR);
        delete __server;
        delete serverSocket;

        return EXIT_FAILURE;
    }

    char *machineIPv4Address = serverSocket->getMachineIPv4Address();

    std::cout << machineIPv4Address  << ":" << port << "\n";

    struct sockaddr_in *serverAddress = serverSocket->IPv4Address(machineIPv4Address, port);

    if (__server->bindServer(serverSocketFD, serverAddress) != 0)
    {
        std::cerr << "Error binding the server!\n";
        perror("bind");

        shutdown(serverSocketFD, SHUT_RDWR);
        delete __server;
        delete serverSocket;
        free(serverAddress);
        delete[] machineIPv4Address;

        return EXIT_FAILURE;
    }

    std::cout << "Server socket bound successfully!\n";

    if (listen(serverSocketFD, 10) == -1)
    {
        shutdown(serverSocketFD, SHUT_RDWR);
        free(serverAddress);
        delete serverSocket;
        delete __server;

        return EXIT_FAILURE;
    }

    __server->__MASTER_THREAD__(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);
    free(serverAddress);
    delete serverSocket;
    delete __server;
    delete[] machineIPv4Address;

    return EXIT_SUCCESS;
}