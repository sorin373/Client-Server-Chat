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

template class Server<char>;

template <typename T> std::atomic<bool> Server<T>::SERVER_RUNNING;

/* db */

template <typename T>
Server<T>::db::db(sql::Driver *driver, sql::Connection *con,
                  const char *hostname, const char *username, const char *password, const char *database)
{
    this->driver = driver;
    this->con = con;
    this->dbCred = new db_cred(hostname, username, password, database);
}

template <typename T>
sql::Connection *Server<T>::db::getCon(void) const noexcept
{
    return con;
}

template <typename T>
sql::Driver *Server<T>::db::getDriver(void) const noexcept
{
    return driver;
}

template <typename T>
class Server<T>::db::db_cred *Server<T>::db::getDB_Cred(void) const noexcept
{
    return dbCred;
}

template <typename T>
Server<T>::db::~db()
{
    this->con->close();
    delete this->con;
    delete dbCred;
}

/* db_cred */

template <typename T>
Server<T>::db::db_cred::db_cred(const char *hostname, const char *username, const char *password, const char *database)
{
    this->hostname = strdup(hostname);
    this->username = strdup(username);
    this->password = strdup(password);
    this->database = strdup(database);
}

template <typename T>
char *Server<T>::db::db_cred::getHostname(void) const noexcept
{
    return hostname;
}

template <typename T>
char *Server<T>::db::db_cred::getUsername(void) const noexcept
{
    return username;
}

template <typename T>
char *Server<T>::db::db_cred::getPassword(void) const noexcept
{
    return password;
}

template <typename T>
char *Server<T>::db::db_cred::getDatabase(void) const noexcept
{
    return database;
}

template <typename T>
int Server<T>::db::db_cred::getCred(char *hostname, char *username, char *password, char *database)
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
Server<T>::db::db_cred::~db_cred()
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

/* ignore page */

Ignore::node::node(const char *ignore_item)
{
    this->next = nullptr;
    this->prev = nullptr;
    this->ignore_item = strdup(ignore_item);
}

char *Ignore::node::getIgnoreItem(void) const noexcept
{
    return ignore_item;
}

Ignore::node::~node()
{
    free(this->ignore_item);
    this->ignore_item = nullptr;
}

Ignore::Ignore()
{
    this->head = nullptr;
    this->tail = nullptr;
}

Ignore::node *Ignore::getHead(void) const noexcept
{
    return head;
}

Ignore::node *Ignore::getTail(void) const noexcept
{
    return tail;
}

inline void Ignore::fetchIgnoreItems(const char *ignore_item)
{
    node *newnode = new node(ignore_item);

    if (head == nullptr)
    {
        head = newnode;
        tail = newnode;
    }
    else
    {
        tail->next = newnode;
        newnode->prev = tail;
        tail = newnode;
    }
}

Ignore::~Ignore()
{
    node *ptr = head;

    while (ptr != nullptr)
    {
        node *temp = ptr;
        ptr = ptr->next;
        delete temp;
    }
}

/* Server */

template <typename T>
Server<T>::Server()
{
    this->db = nullptr;
    this->user = nullptr;
}

template <typename T>
inline int Server<T>::read_ignore_data(void)
{
    std::ifstream file(IGNORE);

    if (!file.is_open())
    {
        std::cerr << "--> Encountered an error while attempting to open the file: " << IGNORE << '\n';
        return EXIT_FAILURE;
    }

    char line[256];

    while (file.getline(line, 256))
        ignore.fetchIgnoreItems(line);

    file.close();

    return EXIT_SUCCESS;
}

template <typename T>
bool Server<T>::acceptConnection(const int serverSocketFD, class acceptedSocket &acceptedSocket)
{
    struct sockaddr_in clientAddress;
    int clientAddressSize = sizeof(clientAddress);

    int clientSocketFD = accept(serverSocketFD, (struct sockaddr *)&clientAddress, (socklen_t *)&clientAddressSize);

    if (clientSocketFD > 0)
    {
        acceptedSocket.getAcceptedSocket(clientAddress, clientSocketFD, clientSocketFD > 0);
        return true;
    }

    return false;
}

char *route = nullptr;
bool changeRoute = false;

template <typename T>
int Server<T>::POSTrequestsHandler(T *buffer, int acceptedSocketFD, ssize_t bytesReceived)
{
    char *charBuffer = reinterpret_cast<char *>(buffer);

    // Upon getting the initial buffer, it establishes the pathway for incoming data until 'changeRoute' is reset again.
    if (changeRoute)
    {
        if (route != nullptr) delete[] route;

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
            if (route[i] == ' ') route[i] = '\0';

        delete[] ptr;

        changeRoute = false;
    }

    if (user->routeManager<T>(buffer, route, acceptedSocketFD, bytesReceived) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

template <typename T>
int Server<T>::GETrequestsHandler(T *buffer, int acceptedSocketFD)
{
    bool USE_DEFAULT_ROUTE = false;

    const char defaultRoute[] = "interface/login.html";
    const char root[] = "interface";

    char *allocatedBuffer = reinterpret_cast<char *>(buffer);
    char *path = nullptr;

    if (allocatedBuffer == nullptr) return EXIT_FAILURE;

    for (int i = 0, n = strlen(allocatedBuffer); i < n; i++)
        if (allocatedBuffer[i] == '/')
        {
            path = &allocatedBuffer[i];
            break;
        }

    if (path == nullptr) USE_DEFAULT_ROUTE = true;

    std::ifstream file;

    if (!USE_DEFAULT_ROUTE)
    {
        for (int i = 0, n = strlen(path); i < n; i++)
            if (path[i] == ' ')
                path[i] = '\0';

        if ((strlen(path) == 1 && path[0] == '/')) USE_DEFAULT_ROUTE = true;

        if (!user->getAuthStatus())
        {
            bool use_default = true;

            for (Ignore::node *ptr = ignore.getHead(); ptr != nullptr; ptr = ptr->next)
            {
                char *temp = ptr->getIgnoreItem();

                if (temp[0] == '.')
                    if (findString(path, temp))
                    {
                        use_default = false;
                        break;
                    }

                if (use_default)
                    if (strcmp(path, ptr->getIgnoreItem()) == 0)
                    {
                        use_default = false;
                        break;
                    }
            }

            if (use_default) USE_DEFAULT_ROUTE = true;
        }

        if (!USE_DEFAULT_ROUTE)
        {
            if (strcmp(path, "/login.html") == 0)
            {
                user->resetAuthStatus();
                user->resetSessionID();
            }

            char fullPath[strlen(root) + strlen(path) + 1] = "";

            if (!findString(path, "interface")) strcpy(fullPath, root);

            strcat(fullPath, path);

            file.open(fullPath, std::ios::binary);
        }
    }

    if (USE_DEFAULT_ROUTE) file.open(defaultRoute, std::ios::binary);

    if (!file.is_open())
    {
        if (DEBUG_FLAG)
            std::cerr << std::setw(5) << " "
                      << "--> Encountered an error while attempting to open the GET file (f_13)\n";
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
                  << "--> Error 01: Failed to send HTTP response: " + response.str();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

char *requestType = nullptr;

template <typename T>
int Server<T>::HTTPrequestsHandler(T *buffer, int acceptedSocketFD, ssize_t bytesReceived)
{
    char *charBuffer = reinterpret_cast<char *>(buffer);
    char *copyBuffer = new char[bytesReceived + 1];

    strncpy(copyBuffer, charBuffer, bytesReceived);
    copyBuffer[bytesReceived] = '\0';

    char *ptr = strstr(copyBuffer, "GET");

    if (ptr == NULL)
    {
        strncpy(copyBuffer, charBuffer, bytesReceived);
        copyBuffer[bytesReceived] = '\0';

        ptr = strstr(copyBuffer, "POST");
    }

    if (ptr != NULL)
    {
        if (requestType != nullptr) delete[] requestType;

        changeRoute = true;

        for (unsigned int i = 0; ptr[i] != '\0'; i++)
            if (ptr[i] == ' ')
            {
                ptr[i] = '\0';
                break;
            }
                

        requestType = new char[strlen(ptr) + 1];

        strcpy(requestType, ptr);
    }

    if (requestType != nullptr && strcasecmp(requestType, "GET") == 0)
        if (GETrequestsHandler(buffer, acceptedSocketFD) == EXIT_FAILURE)
        {
            delete[] copyBuffer;
            return EXIT_FAILURE;
        }

    if (requestType != nullptr && strcasecmp(requestType, "POST") == 0)
        if (POSTrequestsHandler(buffer, acceptedSocketFD, bytesReceived) == EXIT_FAILURE)
        {
            delete[] copyBuffer;
            return EXIT_FAILURE;
        }

    delete[] copyBuffer;

    return EXIT_SUCCESS;
}

template <typename T>
int Server<T>::formatFile(const std::string fileName)
{
    // Open the binary file
    std::ifstream file(BINARY_FILE_TEMP_PATH, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Failed to open (f_11): " << BINARY_FILE_TEMP_PATH << '\n';
        return EXIT_FAILURE;
    }

    // Open the new file that will contain the formatted file data
    std::ofstream outFile(std::string(LOCAL_STORAGE_PATH) + fileName, std::ios::binary);

    if (!outFile.is_open())
    {
        std::cerr << "Failed to open (f_12): " << std::string(LOCAL_STORAGE_PATH) + fileName << '\n';
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

                outFile << line << std::endl;
            }

            break;
        }
    }

    file.close();
    outFile.close();

    // Remove 'temp.bin'
    if (remove(BINARY_FILE_TEMP_PATH) != 0) std::cerr << "Failed to remove: " << BINARY_FILE_TEMP_PATH << "\n";

    return EXIT_SUCCESS;
}

template <typename T>
void Server<T>::postRecv(const int acceptedSocketFD)
{
    char response[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";
    std::string file = user->getFileInQueue();

    if (!file.empty())
    {
        formatFile(file);

        user->addToFileTable(file.c_str(), TOTAL_BYTES_RECV / 1000000.0000);

        user->clearFileInQueue();

        TOTAL_BYTES_RECV = 0;

        if (send(acceptedSocketFD, response, strlen(response), 0) == -1)
            std::cerr << std::setw(5) << " "
                      << "--> Error 02: Failed to send HTTP response.\n";
    }
}

template <typename T>
void Server<T>::receivedDataHandler(const class acceptedSocket socket)
{
    int acceptedSocketFD = socket.getAcceptedSocketFD();

    if (acceptedSocketFD < 0) return;

    T buffer[1025];

    while (true)
    {
        ssize_t bytesReceived = recv(acceptedSocketFD, buffer, sizeof(buffer), 0);

        if (bytesReceived <= 0)
        {
            if (DEBUG_FLAG)
                std::cerr << "\n"
                          << std::setw(5) << " "
                          << "--> Receive failed (R_01): "
                          << socket.getError() << "\n";

            postRecv(acceptedSocketFD);

            break;
        }

        if (DEBUG_FLAG)
        {
            std::cout << "\n";
            underline(75);
        }

        buffer[bytesReceived] = '\0';

        if (DEBUG_FLAG) std::cout << buffer;

        HTTPrequestsHandler(buffer, acceptedSocketFD, bytesReceived);
    }
}

template <typename T>
void Server<T>::receivedDataHandlerThread(const class acceptedSocket socket)
{
    std::thread(&Server::receivedDataHandler, this, socket).detach();
}

template <typename T>
void Server<T>::handleClientConnections(int serverSocketFD, std::atomic<bool> &server_running)
{
    while (server_running)
    {
        // Set up the file descriptor set for select
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(serverSocketFD, &readfds);

        // Set timeout
        struct timeval timeout;
        timeout.tv_sec = 1;  // 1 second timeout
        timeout.tv_usec = 0;

        // Wait for activity on Server socket
        int activity = select(serverSocketFD + 1, &readfds, NULL, NULL, &timeout);

        if (activity > 0)
        {
            acceptedSocket newAcceptedSocket;

            if (!acceptConnection(serverSocketFD, newAcceptedSocket)) continue;

            connectedSockets.push_back(newAcceptedSocket);

            receivedDataHandlerThread(newAcceptedSocket);
        }
        else if (activity == -1)
        {
            perror("Something went wrong!");
            break;
        }
        else continue;
    }

    for (const acceptedSocket &socket : connectedSockets) close(socket.getAcceptedSocketFD());
}

template <typename T>
void Server<T>::consoleListener(std::atomic<bool> &server_running)
{
    underline(75);

    char input[101] = "";

    while (server_running)
    {
        std::cout << std::setw(5) << " "
                  << "--> ";
        std::cin >> input;

        if (strcasecmp(input, "exit") == 0)
        {
            std::cout << std::setw(5) << " "
                      << "--> \nShutting down...\n";

            haltServer();

            break;
        }
    }
}

template <typename T>
void Server<T>::server_easy_init(int serverSocketFD)
{
    if (read_ignore_data() == EXIT_FAILURE) return;

    SERVER_RUNNING = true;

    std::thread workerThread(&Server::handleClientConnections, this, serverSocketFD, std::ref(SERVER_RUNNING));
    std::thread consoleThread(&Server::consoleListener, this, std::ref(SERVER_RUNNING));

    consoleThread.join();
    workerThread.join();
}

template <typename T>
int Server<T>::bindServer(int serverSocketFD, struct sockaddr_in *__serverAddress)
{
    return bind(serverSocketFD, (struct sockaddr *)__serverAddress, sizeof(struct sockaddr_in));
}

template <typename T>
std::vector<class Server<T>::acceptedSocket> Server<T>::getConnectedSockets(void) const noexcept
{
    return connectedSockets;
}

template <typename T>
bool Server<T>::getServerStatus(void) const noexcept
{
    return SERVER_RUNNING;
}

template <typename T>
Ignore Server<T>::getIgnore(void) const noexcept
{
    return ignore;
}

template <typename T>
void Server<T>::haltServer(void) noexcept
{
    this->SERVER_RUNNING = false;
}

template <typename T>
class Server<T>::db *Server<T>::getSQLdatabase(void) const noexcept
{
    return db;
}

template <typename T>
class interface::User *Server<T>::getUser(void) const noexcept
{
    return user;
}

template <typename T>
int Server<T>::database_easy_init(void)
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

    if (Server<T>::db::db_cred::getCred(hostname, username, password, database) == EXIT_FAILURE)
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

        db = new typename Server<T>::db(driver, con, hostname, username, password, database);
        user = new interface::User();
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

    user->SQLfetchUserTable();

    return EXIT_SUCCESS;
}

template <typename T>
Server<T>::~Server()
{
    if (this->db != nullptr)
    {
        delete this->db;
        this->db = nullptr;
    }

    if (this->user != nullptr)
    {
        delete this->user;
        this->user = nullptr;
    }
}

/* acceptedSocket */

template <typename T>
void Server<T>::acceptedSocket::socketCleanup(void) noexcept
{
    this->acceptedSocketFD = -1;
}

template <typename T>
void Server<T>::acceptedSocket::getAcceptedSocket(const struct sockaddr_in ipAddress, const int acceptedSocketFD, const int error)
{
    this->ipAddress = ipAddress;
    this->acceptedSocketFD = acceptedSocketFD;
    this->error = error;
}

template <typename T>
struct sockaddr_in Server<T>::acceptedSocket::getIpAddress(void) const noexcept
{
    return ipAddress;
}

template <typename T>
int Server<T>::acceptedSocket::getError(void) const noexcept
{
    return error;
}

template <typename T>
int Server<T>::acceptedSocket::getAcceptedSocketFD(void) const noexcept
{
    return acceptedSocketFD;
}

int net::INIT(int argc, char *argv[], int addressFamily, int socketType, int protocol)
{
    int port = 0;

    port = getMainArguments(argc, argv);

    if (port == -1)
        return EXIT_FAILURE;

    SocketUtils serverSocket;

    int serverSocketFD = serverSocket.createSocket(addressFamily, socketType, protocol);

    if (serverSocketFD == -1)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Socket creation failed.\n";

        return EXIT_FAILURE;
    }

    server = new Server<char>();

    if (server->database_easy_init() == EXIT_FAILURE)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Database initialization failed.\n";

        shutdown(serverSocketFD, SHUT_RDWR);
        serverSocket.closeSocket(serverSocketFD);

        delete server;

        return EXIT_FAILURE;
    }

    char *machineIPv4Address = serverSocket.getMachineIPv4Address();

    if (machineIPv4Address == nullptr)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Null pointer encountered for machine's IPv4 address.\n";

        shutdown(serverSocketFD, SHUT_RDWR);
        serverSocket.closeSocket(serverSocketFD);

        delete server;

        return EXIT_FAILURE;
    }

    struct sockaddr_in *serverAddress = serverSocket.IPv4Address(machineIPv4Address, port);

    if (server->bindServer(serverSocketFD, serverAddress) != 0)
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
        delete server;
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
              << "--> Access the Server through your web browser via: \e[1m" << machineIPv4Address << ":" << port << "\e[0m\n"
              << std::setw(5) << " "
              << "    In order to shutdown the Server type 'exit' in the console!\n";

    if (listen(serverSocketFD, 10) == -1)
    {
        std::cerr << std::setw(5) << " "
                  << "--> Error: Failed to initiate listening on the Server socket.\n";

        shutdown(serverSocketFD, SHUT_RDWR);
        serverSocket.closeSocket(serverSocketFD);

        free(serverAddress);
        delete server;
        delete[] machineIPv4Address;

        return EXIT_FAILURE;
    }

    server->server_easy_init(serverSocketFD);

    shutdown(serverSocketFD, SHUT_RDWR);
    serverSocket.closeSocket(serverSocketFD);

    free(serverAddress);
    delete server;
    delete[] machineIPv4Address;

    return EXIT_SUCCESS;
}