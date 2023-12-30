<p align="center">
  <img src="assets/orange-logo.png" alt="Project Logo or Banner" width="200" height="200">
</p>

<h3 align="center">

[Homepage](https://github.com/sorin373/HTTP-Server) | [Demo](https://github.com/sorin373/HTTP-Server/tree/main/demo) | [License](https://github.com/sorin373/HTTP-Server/blob/main/LICENSE)

</h3>

In order to complete the CS50 course I was required to build a personal computer science project. As a result I decided to implement a simple HTTP server, which enables users to upload and download files within their local network.

[![License: MIT](https://img.shields.io/badge/License-MIT-orange.svg)](https://github.com/sorin373/HTTP-Server/blob/main/README.md)

# Table of Contents

- [Installation](#installation)
  - [Unix (Debian)](#unix-debian)
  - [Unix (RPM-based)](#unix-rpm-based)
- [Building the executable binaries](#building-the-executable-binaries)
- [Features](#features)
  - [Upload files](#upload-files)
  - [Delete files]()
  - [Create account]()
  - [Change password]()
- [MySql](#mysql-service)
  - [Database initialisation](#database-initialisation)
  - [Create the required tables](#create-the-required-tables)
- [Contact](#contact)

# Installation

## Unix (Debian)

1. Download the Debian package (`.deb` file) from the [GitHub Releases](https://github.com/sorin373/HTTP-Server/releases/tag/v1.0.0) page.
2. Install the package using the software installer
3. By default, SpeedyGo will be installed in the `/usr/httpServer` directory. To run the application, open your terminal here and execute:

```bash
./httpServer [port] [-debug]
```

## Unix (RPM-based)

1. Download the RPM package (`.rpm` file) from the [GitHub Releases](https://github.com/sorin373/HTTP-Server/releases/tag/v1.0.0) page.
2. Install the package using the rpm command:

```bash
sudo rpm -i httpServer-1.0.0-Linux.rpm
```

3. By default, SpeedyGo will be installed in the `/usr/httpServer` directory. To run the application, open your terminal here and execute:

```bash
./httpServer [port] [-debug]
```

# Building the executable binaries

In order to build the application CMake and MySql-Connector-CPP need to be installed.

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

# Features

## Upload files

- The application supports **file uploading** which enables users to share files between machines on a **local network**. Using an HTML **form** element, an **HTTP POST request** is sent to the server containg the file data together with the request metadata. This is later stored in a `temp.bin` file for later formatting. After receiving all the HTTP request contents, the file data is extracted and stored locally in a file, ensuring it has the appropriate name and extension. The file formatting consists in:
    
1) Retrieving the file name together with the file extension

    ```C++
    if (findString(buffer, "filename="))
    {
        fileName.clear();

        const std::string t_buffer = std::string(buffer);

        // define the pattern
        std::regex fileNameRegex(R"(filename=\"([^\"]+)\")");
        std::smatch match;

        // searches in the string 't_buffer' for matches
        if (std::regex_search(t_buffer, match, fileNameRegex))
            fileName = match.str(1);

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
            for (unsigned int i = 0, n = fileName.length(); i < n; i++)
                if (fileName[i] == ' ')
                    fileName[i] = '_';
        }
        
        // Adds the new file in a queue to be inserted in the database
        addFileInQueue(fileName);
    }
    ```

2) Extracting the file content located between the two boundaries: ***------WebKitFormBoundary***

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
    if (remove(BINARY_FILE_TEMP_PATH) != 0)
        std::cerr << "Failed to removed temp.bin!\n";
    ```
