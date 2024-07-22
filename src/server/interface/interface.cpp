#include "interface.hpp"
#include "../global.hpp"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <string>
#include <regex>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>

constexpr char INDEX_HTML_PATH[] =       "interface/index.html";
constexpr char BINARY_FILE_TEMP_PATH[] = "interface/storage/temp.bin";
constexpr char LOCAL_STORAGE_PATH[] =    "interface/storage/";

using namespace net;
using namespace net::interface;

User::userCredentials::userCredentials(const char *username, const char *password, const int id)
{
    this->username = strdup(username);
    this->password = strdup(password);
    this->id = id;
}

char *User::userCredentials::getUsername(void) const noexcept
{
    return username;
}

char *User::userCredentials::getPassword(void) const noexcept
{
    return password;
}

int User::userCredentials::getId(void) const noexcept
{
    return id;
}

User::User()
{
    this->AUTH_STATUS = false;
    this->SESSION_ID = -1;

    m_sql_db = (sql_db*)malloc(sizeof(sql_db));
}

std::vector<class User::userCredentials> User::getUserCredentials(void) const noexcept
{
    return uc;
}

std::vector<class User::userFiles> User::getUserFiles(void) const noexcept
{
    return uf;
}

std::string User::getFileInQueue(void) const noexcept
{
    return fileInQueue;
}

int User::getSessionID(void) const noexcept
{
    return SESSION_ID;
}

bool User::getAuthStatus(void) const noexcept
{
    return AUTH_STATUS;
}

void User::resetAuthStatus(void) noexcept
{
    this->AUTH_STATUS = false;
}

void User::resetSessionID(void) noexcept
{
    this->SESSION_ID = -1;
}

void User::clearUserCredentials(void) noexcept
{
    for (userCredentials  &t_uc : uc)
    {
        free(t_uc.getUsername());
        free(t_uc.getPassword());
    }

    uc.clear();
}

void User::clearUserFiles(void) noexcept
{
    for (userFiles &t_uf : uf) 
    {
        free(t_uf.getFileName());
        free(t_uf.getDate());
    }   

    uf.clear();
}

void User::clearFileInQueue(void) noexcept
{
    fileInQueue.clear();
}

void User::addToUserCredentials(const User::userCredentials t_uc) noexcept
{
    uc.push_back(t_uc);
}

void User::addToUserFiles(const User::userFiles t_uf) noexcept
{
    uf.push_back(t_uf);
}

void User::addFileInQueue(const std::string fileName) noexcept
{
    this->fileInQueue = fileName;
}

/* UserFiles table */

User::userFiles::userFiles(const char *fileName, const int userID, const int fileID, const double fileSize, const char *date)
{
    this->fileName = strdup(fileName);
    this->userID = userID;
    this->fileID = fileID;
    this->fileSize = fileSize;
    this->date = strdup(date);
}

char *User::userFiles::getFileName(void) const noexcept
{
    return fileName;
}

int User::userFiles::getUserID(void) const noexcept
{
    return userID;
}

int User::userFiles::getFileID(void) const noexcept
{
    return fileID;
}

double User::userFiles::getFileSize(void) const noexcept
{
    return fileSize;
}

char *User::userFiles::getDate(void) const noexcept
{
    return date;
}

void User::buildIndexHTML(void)
{
    std::ofstream index_html(INDEX_HTML_PATH);

    if (!index_html.is_open())
    {
        std::cerr << std::setw(5) << " "
                  << "Error: Failed to open file: " << INDEX_HTML_PATH << "\n";
        return;
    }

    char firstHTML[] = R"(<!DOCTYPE html>
                        <html lang="en">

                        <head>
                            <meta charset="UTF-8"/>
                            <meta name="viewport" content="width=device-width, initial-scale=1.0"/>

                            <title>http-Server</title>
                            
                            <link crossorigin="anonymous" href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css"
                                integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" rel="stylesheet"/>
                            <script crossorigin="anonymous" src="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js"
                                integrity="sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p"></script>

                            <link rel="icon" href="static/assets/logo.png"/>
                            <link href="static/stylesheet/index.css" rel="stylesheet"/>
                            <script src="static/javascript/deleteFileRequest.js"></script>
                            <script src="static/javascript/addFileRequest.js"></script>
                            <script src="static/javascript/search.js"></script>

                            <style>
                                a {
                                    text-decoration: none;
                                    font-size: 15pt;
                                    padding: 1px 6px;
                                    color: rgb(66, 66, 66) !important;
                                }

                                a:hover {
                                    color: #005ed8 !important;
                                }
                            </style>
                        </head>

                        <body>
                            <div class="main-container">
                                <div class="title-container">
                                    <input type="text" id="inputSearch" autofocus onkeyup="search() " placeholder="Search file">
                                    <a class="btn btn-primary ext-ref" href="login.html">Logout</a>
                                </div>
                                <div class="container">
                                    <table id="tableID" class="table" style="margin: auto;">
                                        <thead>
                                            <tr>
                                                <th scope="col">#</th>
                                                <th scope="col">File Name</th>
                                                <th scope="col">File Size(mb)</th>
                                                <th scope="col">Upload Date</th>
                                                <th scope="col" style="width: 85px;"></th>
                                            </tr>
                                        </thead>

                                        <tbody>)";

    index_html << firstHTML;

    char fileName[256] = "";

    for (size_t i = 0, n = uf.size(); i < n; i++)
    {
        strcpy(fileName, uf[i].getFileName());

        std::string td1 = "<tr><td scope=\"row\" class=\"left-column\">" + std::to_string(uf[i].getFileID()) + "</td>";
        std::string td2 = "<td scope=\"row\">" + std::string(uf[i].getFileName()) + "</td>";
        std::string td3 = "<td scope=\"row\">" + std::to_string(uf[i].getFileSize()) + "</td>";
        std::string td4 = "<td scope=\"row\">" + std::string(uf[i].getDate()) + "</td>";

        std::string td = R"(<td class="td-btn right-column">
                            <button type="button" class="delete-btn">
                                <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-trash" viewBox="0 0 16 16">
                                    <path
                                        d="M5.5 5.5A.5.5 0 0 1 6 6v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5m2.5 0a.5.5 0 0 1 .5.5v6a.5.5 0 0 1-1 0V6a.5.5 0 0 1 .5-.5m3 .5a.5.5 0 0 0-1 0v6a.5.5 0 0 0 1 0z" />
                                    <path
                                        d="M14.5 3a1 1 0 0 1-1 1H13v9a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V4h-.5a1 1 0 0 1-1-1V2a1 1 0 0 1 1-1H6a1 1 0 0 1 1-1h2a1 1 0 0 1 1 1h3.5a1 1 0 0 1 1 1zM4.118 4 4 4.059V13a1 1 0 0 0 1 1h6a1 1 0 0 0 1-1V4.059L11.882 4zM2.5 3h11V2h-11z" />
                                </svg>
                            </button>)";

        td = td + "<a href=\"storage/" + uf[i].getFileName() + "\" download=\"" + uf[i].getFileName() + "\">";

        td = td + R"(<svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" fill="currentColor" class="bi bi-download" viewBox="0 0 16 16">
                                    <path d="M.5 9.9a.5.5 0 0 1 .5.5v2.5a1 1 0 0 0 1 1h12a1 1 0 0 0 1-1v-2.5a.5.5 0 0 1 1 0v2.5a2 2 0 0 1-2 2H2a2 2 0 0 1-2-2v-2.5a.5.5 0 0 1 .5-.5"/>
                                    <path d="M7.646 11.854a.5.5 0 0 0 .708 0l3-3a.5.5 0 0 0-.708-.708L8.5 10.293V1.5a.5.5 0 0 0-1 0v8.793L5.354 8.146a.5.5 0 1 0-.708.708l3 3z"/>
                                </svg>
                            </a></td></tr>)";

        std::string HTMLcontent = td1 + td2 + td3 + td4 + td;

        index_html << HTMLcontent;
    }

    char HTMLclose[] = R"(
        </tbody>
        </table>
        </div>

        <form action="/addFile" method="post" id="addFileForm" enctype="multipart/form-data">
        <label for="file-upload-btn" id="file-upload">Select a file</label>
        <input id="file-upload-btn" type="file" name="filename" onchange="displayFileName(this) ">
        <br>
        <label id="file-name-label">No file selected</label>
        <br>
        <input type="submit" class="btn btn-primary" value="Upload">
        </form>

        <progress id="uploadProgress" value="0" max="100"></progress>
        <div id="uploadStatus"></div>
        
        <script>
            function displayFileName(input) {
                var fileName = input.files[0].name;
                var fileNameLabel = document.getElementById('file-name-label');
                fileNameLabel.innerText = fileName;
            }
        </script>

        </div>

        <div class="footer">
            <p>Copyright (c) 2023 Sorin Tudose | </p>
            <a href="https://github.com/sorin373/HTTP-Server">GitHub</a>
        </div>

        </body>
        </html>
     )";

    index_html << HTMLclose;

    index_html.close();
}

void User::SQLfetchUserTable(void)
{
    clearUserCredentials();

    sql::ResultSet *res = m_sql_db->send_query("SELECT * FROM user");

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

        // create an obj which we are pushing into the vector
        User::userCredentials t_uc(username, password, id);

        addToUserCredentials(t_uc);

        free(username);
        free(password);
    }

    res->close();
    delete res;
}

void User::SQLfetchFileTable(void)
{
    clearUserFiles();

    sql::ResultSet *res = m_sql_db->send_query("SELECT * FROM file WHERE user_id=" + std::to_string(getSessionID()));

    while (res->next())
    {
        int userID = res->getInt("user_id");
        int fileID = res->getInt("file_id");
        double fileSize = res->getDouble("size");
        std::string date = res->getString("date");

        sql::SQLString sqlstr;
        sqlstr = res->getString("name");
        char *fileName = (char *)malloc(sqlstr.asStdString().length() + 1);
        strcpy(fileName, sqlstr.asStdString().c_str());

        sqlstr = res->getString("date");
        char *t_date = (char *)malloc(sqlstr.asStdString().length() + 1);
        strcpy(t_date, sqlstr.asStdString().c_str());

        User::userFiles t_uf(fileName, userID, fileID, fileSize, t_date);

        addToUserFiles(t_uf);

        free(fileName);
        free(t_date);
    }

    res->close();
    delete res;
}

int User::addToFileTable(const char *fileName, const double fileSize)
{
    bool found = false;

    for (const interface::User::userFiles &t_uf : uf)
        if (strcmp(t_uf.getFileName(), fileName) == 0)
        {
            found = true;
            break;
        }

    if (found) return EXIT_SUCCESS;

    int maxID = 0;

    sql::ResultSet *res = m_sql_db->send_query("SELECT file_id FROM file");

    while (res->next())
    {
        int fileID = res->getInt("file_id");

        if (fileID > maxID) maxID = fileID;
    }

    res->close();
    delete res;

    res = m_sql_db->send_query("SELECT sysdate() As date FROM dual");

    res->next();

    std::string date = res->getString("date");

    res->close();
    delete res;

    try
    {
        std::string query = "INSERT INTO file (user_id, file_id, name, size, date) VALUES (?, ?, ?, ?, ?)";

        sql::PreparedStatement *prepStmt = m_sql_db->get_connection()->prepareStatement(query);

        prepStmt->setInt(1, getSessionID());
        prepStmt->setInt(2, maxID + 1);
        prepStmt->setString(3, std::string(fileName));
        prepStmt->setDouble(4, fileSize);
        prepStmt->setString(5, date);

        prepStmt->executeUpdate();

        prepStmt->close();
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

    buildIndexHTML();

    return EXIT_SUCCESS;
}

bool User::validateCredentials(const char username[], const char password[])
{
    for (userCredentials &t_uc : uc)
        if (strcmp(username, t_uc.getUsername()) == 0 && strcmp(password, t_uc.getPassword()) == 0)
        {
            this->SESSION_ID = t_uc.getId();
            return true;
        }

    return false;
}

bool User::findUsername(const char username[])
{
    for (userCredentials &t_uc : uc)
        if (strcmp(username, t_uc.getUsername()) == 0) return true;

    return false;
}

int User::routeManager(void *buffer, char *route, int acceptedSocketFD, ssize_t bytesReceived)
{  
    char *charBuffer = reinterpret_cast<char *>(buffer);

    if (strstr(route, "/userlogin"))
        if (loginRoute(charBuffer, acceptedSocketFD) == EXIT_FAILURE) return EXIT_FAILURE;

    if (strstr(route, "/addFile"))
    {
        // uint8_t used, because strict byte-level representation is needed
        BYTE *byteBuffer = reinterpret_cast<BYTE *>(buffer); 

        if (addFilesRoute(charBuffer, byteBuffer, acceptedSocketFD, bytesReceived) == EXIT_FAILURE) return EXIT_FAILURE;
    }
        
    if (strstr(route, "/change_password"))
        if (changePasswordRoute(charBuffer, acceptedSocketFD) == EXIT_FAILURE) return EXIT_FAILURE;

    if (strstr(route, "/create_account"))
        if (createAccountRoute(charBuffer, acceptedSocketFD) == EXIT_FAILURE) return EXIT_FAILURE;

    if (strstr(route, "/delete_file"))
        if (deleteFileRoute(charBuffer, acceptedSocketFD) == EXIT_FAILURE) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int User::loginRoute(char *buffer, int acceptedSocketFileDescriptor)
{
    char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";
    char unauthorized[] = "HTTP/1.1 302 Found\r\nLocation: /login.html\r\nConnection: close\r\n\r\n";

    char *temp_username, *temp_password, *ptr = NULL;

    ptr = strstr(buffer, "username=");
    ptr = strtok(ptr, "&");

    if (ptr != NULL)
        for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
            if (ptr[i] == '=')
            {
                temp_username = &ptr[++i];
                break;
            }

    if (temp_username == nullptr) return EXIT_FAILURE;

    if (strlen(temp_username) > NET_USERNAME_LENGHT) return EXIT_FAILURE;

    ptr = strtok(NULL, " ");

    if (ptr != NULL)
        for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
            if (ptr[i] == '=')
            {
                temp_password = &ptr[++i];
                break;
            }

    if (temp_password == nullptr) return EXIT_FAILURE;

    if (strlen(temp_password) > NET_PASSWORD_LENGHT) return EXIT_FAILURE;

    if (!validateCredentials(temp_username, temp_password))
        send_to_client(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized));

    this->AUTH_STATUS = true;

    send_to_client(acceptedSocketFileDescriptor, authorized, strlen(authorized));

    SQLfetchFileTable();

    buildIndexHTML();

    return EXIT_SUCCESS;
}

std::string fileName;
int fileCount;

int User::addFilesRoute(const char *buffer, const uint8_t *byteBuffer, int acceptedSocketFileDescriptor, ssize_t bytesReceived)
{
    TOTAL_BYTES_RECV += bytesReceived;

    if (strstr(buffer, "filename=") != nullptr)
    {
        fileName.clear();

        std::regex fileNameRegex(R"(filename=\"([^\"]+)\")");
        std::smatch match;

        const std::string t_buffer = std::string(buffer);
        
        // searches in the string 't_buffer' for matches
        if (std::regex_search(t_buffer, match, fileNameRegex)) fileName = match.str(1);

        // Variable used to generate unique name for files which do not have a valid file name
        fileCount++;

        if (fileName.length() > 40)
        {
            std::string fileExtension;
            std::string newName;

            newName = "Upload" + std::to_string(fileCount);

            size_t pos = fileName.find(".");

            if (pos != std::string::npos)
            {
                fileExtension = fileName.substr(pos);
                newName = newName + fileExtension;
            }

            fileName = newName;
        }
        else
        {
            // Replacing spaces with underlines
            for (unsigned int i = 0, n = fileName.length(); i < n; i++) if (fileName[i] == ' ') fileName[i] = '_';
        }

        // Adds the new file in the queue to be inserted in the database
        addFileInQueue(fileName);
    }

    FILE *file = fopen(BINARY_FILE_TEMP_PATH, APPEND_BINARY);

    if (file != NULL)
    {
        fwrite(byteBuffer, sizeof(uint8_t), bytesReceived, file);
        fclose(file);
    }
    else
    {
        std::cerr << "Failed to open file: " << BINARY_FILE_TEMP_PATH << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int User::format_file(const std::string file_name)
{
    // Open the binary file
    std::ifstream file(BINARY_FILE_TEMP_PATH, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "==F== Failed to open (f_11): " << BINARY_FILE_TEMP_PATH << '\n';
        return EXIT_FAILURE;
    }

    // Open the new file that will contain the formatted file data
    std::ofstream outFile(std::string(LOCAL_STORAGE_PATH) + fileName, std::ios::binary);

    if (!outFile.is_open())
    {
        std::cerr << "==F== Failed to open (f_12): " << std::string(LOCAL_STORAGE_PATH) + fileName << '\n';
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
    if (remove(BINARY_FILE_TEMP_PATH) != 0) 
        std::cerr << "==F== Failed to remove: '" << BINARY_FILE_TEMP_PATH << "'!\n";

    return EXIT_SUCCESS;
}

int User::changePasswordRoute(char *buffer, int acceptedSocketFileDescriptor)
{
    char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /login.html\r\nConnection: close\r\n\r\n";
    char unauthorized[] = "HTTP/1.1 302 Found\r\nLocation: /apology.html\r\nConnection: close\r\n\r\n";

    char *username = nullptr, *oldPassword = nullptr, *newPassword = nullptr, *confirmation = nullptr, *ptr = NULL;

    ptr = strstr(buffer, "username=");
    ptr = strtok(ptr, "&");

    if (ptr != NULL)
        for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
            if (ptr[i] == '=')
            {
                username = &ptr[++i];
                break;
            }

    if (username == nullptr) return EXIT_FAILURE;

    if (strlen(username) > NET_USERNAME_LENGHT) return EXIT_FAILURE;

    ptr = strtok(NULL, "&");

    if (ptr != NULL)
        for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
            if (ptr[i] == '=')
            {
                oldPassword = &ptr[++i];
                break;
            }

    if (oldPassword == nullptr) return EXIT_FAILURE;

    if (strlen(oldPassword) > NET_PASSWORD_LENGHT) return EXIT_FAILURE;

    ptr = strtok(NULL, "&");

    if (ptr != NULL)
        for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
            if (ptr[i] == '=')
            {
                newPassword = &ptr[++i];
                break;
            }

    if (newPassword == nullptr) return EXIT_FAILURE;

    if (strlen(newPassword) > NET_PASSWORD_LENGHT) return EXIT_FAILURE;

    ptr = strtok(NULL, " ");

    if (ptr != NULL)
        for (unsigned int i = 0, len = strlen(ptr); i < len; i++)
            if (ptr[i] == '=')
            {
                confirmation = &ptr[++i];
                break;
            }

    if (confirmation == nullptr) return EXIT_FAILURE;

    if (strlen(confirmation) > strlen(newPassword)) return EXIT_FAILURE;

    // Check if old credentials are valid
    if (!validateCredentials(username, oldPassword))
    {
        send_to_client(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized) + 1);

        return EXIT_SUCCESS;
    }

    // Check if the new password is the same as the confirmation
    if (strcmp(newPassword, confirmation) != 0)
    {
        send_to_client(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized) + 1);

        return EXIT_SUCCESS;
    }

    // Prepare query to update the User
    std::string query = "UPDATE user SET password=(?) WHERE username=(?)";
    sql::PreparedStatement *prepStmt = m_sql_db->get_connection()->prepareStatement(query);

    prepStmt->setString(1, std::string(newPassword));
    prepStmt->setString(2, std::string(username));

    // Execute the query
    prepStmt->executeUpdate();

    delete prepStmt;

    // Fetch the User table containg the updated data
    SQLfetchUserTable();

    send_to_client(acceptedSocketFileDescriptor, authorized, strlen(authorized) + 1);

    return EXIT_SUCCESS;
}

int User::createAccountRoute(char *buffer, int acceptedSocketFileDescriptor)
{
    char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /login.html\r\nConnection: close\r\n\r\n";
    char unauthorized[] = "HTTP/1.1 302 Found\r\nLocation: /apology.html\r\nConnection: close\r\n\r\n";

    char *username = nullptr, *password = nullptr, *confirmation = nullptr, *ptr = NULL;

    ptr = strstr(buffer, "username=");
    ptr = strtok(ptr, "&");

    if (ptr != NULL)
        for (unsigned int i = 0, n = strlen(ptr); i < n; i++)
            if (ptr[i] == '=')
            {
                username = &ptr[++i];
                break;
            }

    if (username == nullptr) return EXIT_FAILURE;

    if (strlen(username) > NET_USERNAME_LENGHT) return EXIT_FAILURE;

    ptr = strtok(NULL, "&");

    if (ptr != NULL)
        for (unsigned int i = 0, n = strlen(ptr); i < n; i++)
            if (ptr[i] == '=')
            {
                password = &ptr[++i];
                break;
            }

    if (password == nullptr) return EXIT_FAILURE;

    if (strlen(password) > NET_PASSWORD_LENGHT) return EXIT_FAILURE;

    ptr = strtok(NULL, " ");

    if (ptr != NULL)
        for (unsigned int i = 0, n = strlen(ptr); i < n; i++)
            if (ptr[i] == '=')
            {
                confirmation = &ptr[++i];
                break;
            }

    if (confirmation == nullptr)
        return EXIT_FAILURE;

    if (strlen(confirmation) > strlen(password)) return EXIT_FAILURE;

    // Check if the username already exists in the database
    if (findUsername(username))
    {
        send_to_client(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized) + 1);

        return EXIT_SUCCESS;    
    }

    // Check if the password is the same as the confirmation
    if (strcmp(password, confirmation) != 0)
    {
        send_to_client(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized) + 1);

        return EXIT_SUCCESS;
    }

    // Prepare query to insert the new account information
    std::string query = "INSERT INTO user VALUES (?, ?, ?)";
    sql::PreparedStatement *prepStmt = m_sql_db->get_connection()->prepareStatement(query);

    prepStmt->setInt(1, uc.size());
    prepStmt->setString(3, std::string(password));
    prepStmt->setString(2, std::string(username));

    // Execute the query
    prepStmt->executeUpdate();

    delete prepStmt;

    // Fetch the User table containg the updated data
    SQLfetchUserTable();

    send_to_client(acceptedSocketFileDescriptor, authorized, strlen(authorized) + 1);

    return EXIT_SUCCESS;
}

int User::deleteFileRoute(char *buffer, int acceptedSocketFileDescriptor)
{
    char authorized[] = "HTTP/1.1 302 Found\r\nLocation: /index.html\r\nConnection: close\r\n\r\n";

    char *ptr = strstr(buffer, "fileID="), *cfileID = nullptr;
    int fileID = -1;

    if (ptr != nullptr)
        for (unsigned int i = 0, n = strlen(ptr); i < n; i++)
            if (ptr[i] == '=')
            {
                cfileID = &ptr[++i];
                break;
            }

    if (cfileID != nullptr) fileID = atoi(cfileID);

    // Fetch the file name using the file ID for later use
    std::string query = "SELECT name FROM file WHERE file_id=(?)";
    sql::ResultSet *res = nullptr;

    sql::PreparedStatement *prepStmt = m_sql_db->get_connection()->prepareStatement(query);

    prepStmt->setInt(1, fileID);
    res = prepStmt->executeQuery();

    while (res->next()) std::string fileName = res->getString("name");

    delete res;
    delete prepStmt;

    // Delete the file from the database using the file ID
    std::string deleteQuery = "DELETE FROM file WHERE file_id=(?)";

    prepStmt = m_sql_db->get_connection()->prepareStatement(deleteQuery);

    prepStmt->setInt(1, fileID);
    prepStmt->executeUpdate();

    delete prepStmt;

    SQLfetchFileTable();
    buildIndexHTML();

    // Using the file name remove the file from local storage
    std::string fileToDelete = std::string(LOCAL_STORAGE_PATH) + fileName;
    remove(fileToDelete.c_str());

    send_to_client(acceptedSocketFileDescriptor, authorized, strlen(authorized) + 1);

    return EXIT_SUCCESS;
}

User::~User()
{
    for (userCredentials  &t_uc : uc)
    {
        free(t_uc.getUsername());
        free(t_uc.getPassword());
    }

    uc.clear();

    for (userFiles &t_uf : uf) 
    {
        free(t_uf.getFileName());
        free(t_uf.getDate());
    }

    uf.clear();

    free(m_sql_db);
}