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
    - [Upload files]()
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