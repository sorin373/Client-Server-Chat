<p align="center">
  <img src="assets/orange-logo.png" alt="Project Logo or Banner" width="200" height="200">
</p>

<h3 align="center">

[Homepage](https://github.com/sorin373/HTTP-Server) | [Demo](https://github.com/sorin373/HTTP-Server/tree/main/demo) | [YT Presentation](https://youtu.be/OeKAQjgdvNc) | [License](https://github.com/sorin373/HTTP-Server/blob/main/LICENSE)

</h3>

In order to complete the CS50 course I was required to build a personal computer science project. As a result, I decided to implement a simple HTTP server, which enables users to upload and download files within their local network.

[![License: MIT](https://img.shields.io/badge/License-MIT-orange.svg)](https://github.com/sorin373/HTTP-Server/blob/main/README.md)

# Table of Contents

- [Installation](#installation)
  - [Unix (Debian)](#unix-debian)
  - [Unix (RPM-based)](#unix-rpm-based)
- [Building the executable binaries](#building-the-executable-binaries)
- [Server functionalities](#server-functionalities)
- [Features](#features)
  - [Upload files](#upload-files)
  - [Delete files](#delete-files)
  - [Create account](#create-account)
  - [Change password](#change-password)
- [MySql-Database](#mysql-database)
- [Bibliography](#bibliography)
- [Contact](#contact)

# Installation

## Unix (Debian)

1. Download the Debian package (`.deb` file) from the [GitHub Releases](https://github.com/sorin373/HTTP-Server/releases/tag/v1.0.0) page.
2. Install the package using the software installer
3. By default, the installation path is the `/usr/httpServer` directory. To run the application, open your terminal here and execute:

```bash
./httpServer [port] [-debug]
```

## Unix (RPM-based)

1. Download the RPM package (`.rpm` file) from the [GitHub Releases](https://github.com/sorin373/HTTP-Server/releases/tag/v1.0.0) page.
2. Install the package using the rpm command:

```bash
sudo rpm -i httpServer-1.0.0-Linux.rpm
```

3. By default, the installation path is the `/usr/httpServer` directory. To run the application, open your terminal here and execute:

```bash
./httpServer [port] [-debug]
```

# Building the executable binaries

In order to build the application [CMake](https://cmake.org/download/) and **MySql-Connector-CPP** need to be installed.

```bash
sudo apt-get update && sudo apt-get upgrade
```

1. **Install** [CMake](https://cmake.org/download/)

```bash
sudo snap install cmake --classic
```

2. **MySql-Connector-CPP**

```bash
sudo apt-get install libmysqlcppconn-dev
```

- After that, download the zip file containing the latest project files from the GitHub repository, open the folder in Visual Studio Code and use the CMake commands to build the application (or install the [CMake](https://marketplace.visualstudio.com/items?itemName=twxs.cmake) and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extensions).

# Server functionalities

- This HTTP server has the ability to distinguish between `GET` and `POST` requests. It automatically sends the request to the correct function where it is processed.
- The server runs on a `thread`, detached from the `main` function, together with another `thread` that listens for console input. 
- When the binding process is performed the user's internet address is read automatically by running `ifconfig` in the background. The **address**, together with the **port number** will be displayed in the console if the server starts successfully, for the user to paste them in the browser.
- The default port number, the server listens to is **`8080`**.
- The server supports multiple connections at the same time, the connected sockets being stored in a vector.
- When running the application you can specify any valid port number if you wish to override the default one. Moreover, when the application is also ran using the `-debug` flag, the server admin (this only means that `-debug` only affects the console output on the machine, the server was started) can vizualize the HTTP requests and serveral other error messages. This, however, does not mean that when in normal mode there are no errors being shown on the screen.
- Last but not least, the server displays any SQL errors produced by the program.

> **Note** For more information about the sever features there is more documentation in the **[server header file](https://github.com/sorin373/HTTP-Server/blob/main/src/server/serverUtils.hpp)**.

# Features

> **Note**: All functions that are related to the routing process are also documented in the **[interface header](https://github.com/sorin373/HTTP-Server/blob/main/src/server/interface/interface.hpp)**.

## Upload files

- The application supports **file uploading** which enables users to share files between machines on a **local network**. Using an HTML **form** element, an **HTTP POST request** is sent to the server containg the file data together with the request metadata. This is later stored in a `temp.bin` file for later formatting. After receiving all the HTTP request contents, the file data is extracted and stored locally in a file, ensuring it has the appropriate name and extension. The file formatting consists in:
    
1) Retrieving the **file name** together with the **file extension**

    ```C++
    if (findString(buffer, "filename="))
    {
        fileName.clear();

        const std::string t_buffer = std::string(buffer);

        // define the pattern
        std::regex fileNameRegex(R"(filename=\"([^\"]+)\")");
        std::smatch match;

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
    ```

2) Extracting the **file content** located between the **two boundaries** (`------WebKitFormBoundary`)

    ```C++
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
    ```

## Delete files

- The user has the ability to delete the files added to the server. This is done by clicking a button, which in return sends an **HTTP POST** request to the server. This request contains the **file ID** which the application uses to delete the file from the database. 
- Moreover, after the file is removed `index.html` is rebuilt and the `file` table is fetched.

    ```C++
    // Fetch the file name using the file ID for later use
    std::string query = "SELECT name FROM file WHERE file_id=(?)";
    sql::ResultSet *res = nullptr;

    sql::PreparedStatement *prepStmt = server->getSQLdatabase()->getCon()->prepareStatement(query);

    prepStmt->setInt(1, fileID);
    res = prepStmt->executeQuery();

    while (res->next()) std::string fileName = res->getString("name");

    delete res;
    delete prepStmt;

    // Delete the file from the database using the file ID
    std::string deleteQuery = "DELETE FROM file WHERE file_id=(?)";

    prepStmt = server->getSQLdatabase()->getCon()->prepareStatement(deleteQuery);

    prepStmt->setInt(1, fileID);
    prepStmt->executeUpdate();

    delete prepStmt;

    SQLfetchFileTable();
    buildIndexHTML();

    // Using the file name remove the file from local storage
    std::string fileToDelete = std::string(LOCAL_STORAGE_PATH) + fileName;
    remove(fileToDelete.c_str());
    ```

## Create account

- In addition, users are also able to create a new account on the server. The `createAccount.html` page is prebuilt and it is fetched using a **HTTP GET** request sent by the browser. After correctly filling in the form, an **HTTP POST** request is sent containg all the reuquired data for an account to be opened: `Username`, `Password`, and the `Password Confirmation`. Once these are validated, the information about the user account is insterted into the database and the user is redirected back to the login. 
- It is important to know that the user accounts can not yet be deleted once they are created and that the usernames are unique throughout the database.

    ```C++
    // Check if the username already exists in the database
    if (findUsername(username))
    {
        if (send(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized), 0) == -1)
        {
            std::cerr << std::setw(5) << " "
                      << "--> Error: Failed to send response.\n";
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;    
    }

    // Check if the password is the same as the confirmation
    if (strcmp(password, confirmation) != 0)
    {
        if (send(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized), 0) == -1)
        {
            std::cerr << std::setw(5) << " "
                      << "--> Error: Failed to send response.\n";
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    // Prepare query to insert the new account information
    std::string query = "INSERT INTO User VALUES (?, ?, ?)";
    sql::PreparedStatement *prepStmt = server->getSQLdatabase()->getCon()->prepareStatement(query);

    prepStmt->setInt(1, uc.size());
    prepStmt->setString(3, std::string(password));
    prepStmt->setString(2, std::string(username));

    // Execute the query
    prepStmt->executeUpdate();

    delete prepStmt;

    // Fetch the User table containg the updated data
    SQLfetchUserTable();
    ```

## Change password

- Finally, users can change their password. This can be done ONLY IF the current password has not been forgotten.
- On the `login.html` page there is a button when clicked sends an **HTTP GET** request to the server in order to fetch the `changePassword.html` file. After reaching this web page the users needs to fill in another form consiting of: `username`, `Old Password`, `New Password` and the `New Password Confirmation`.
If these fields contain valid pieces of data, the new user account's password will be updated in the database.

    ```C++
    // Check if old credentials are valid
    if (!validateCredentials(username, oldPassword))
    {
        if (send(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized), 0) == -1)
        {
            std::cerr << std::setw(5) << " "
                      << "--> Error: Failed to send response.\n";
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    // Check if the new password is the same as the confirmation
    if (strcmp(newPassword, confirmation) != 0)
    {
        if (send(acceptedSocketFileDescriptor, unauthorized, strlen(unauthorized), 0) == -1)
        {
            std::cerr << std::setw(5) << " "
                      << "--> Error: Failed to send response.\n";
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    // Prepare query to update the User
    std::string query = "UPDATE User SET password=(?) WHERE username=(?)";
    sql::PreparedStatement *prepStmt = server->getSQLdatabase()->getCon()->prepareStatement(query);

    prepStmt->setString(1, std::string(newPassword));
    prepStmt->setString(2, std::string(username));

    // Execute the query
    prepStmt->executeUpdate();

    delete prepStmt;

    // Fetch the User table containg the updated data
    SQLfetchUserTable();
    ```

<hr>

> **Note**: If the form on each page is submitted correctly the user will be redirected to the `login.html` page. However, if the data is incorrect the user should be redirected to `apology.html`.

> **Note**: All input fields are checked for correct length using `C++` logic in order to avoid any buffer overflows.

<hr>

# MySql-Database

![img](/assets/ERD.png)

- The `httpServer` database contains two tables: the `file` table and the `user` table.
- The relationship between the tables is `One-to-Many`.
- My application communicates with the **MySql database** using the **MySql-Connector-CPP** library.

- Before running the application a local MySql database, which will house the necessary data, needs to be created (either commands or the MySql Workbench can be used for this task).

- I provide the `table formats` and the `self-contained file` in the [demo](https://github.com/sorin373/HTTP-Server/tree/main/demo) folder for a quicker setup.

<hr>

> **Note**: I do not provide any sample data. The database is meant to be empty when first used. <br> Users can create accounts, add files and delete files when using the application.

> **Note**: It is highly important that the table names are kept, as they are hard coded in the program. Having said that the database name is up to you as it is required to enter it when the application is ran.

<hr>

# Bibliography

- https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-getting-started-examples.html
- https://getbootstrap.com/docs/5.0/getting-started/introduction/
- https://www.linuxhowtos.org/C_C++/socket.htm
- "*C++ Network Programming Volume 1*", written by Douglas C.Schmidt and Stephen D.Huston

# Contact

I hope this project was beneficial and helpful. If you want to get in touch with me, you can do so through my personal email: <br> <br> sorin.andrei.tudose@gmail.com<br><br>Wishing you a delightful and productive day! 