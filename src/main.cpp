/**
 *
 * @file           main.cpp
 *
 * @copyright      MIT License
 *
 *                 Copyright (c) 2023 Sorin Tudose
 *
 *                 Permission is hereby granted, free of charge, to any person obtaining a copy
 *                 of this software and associated documentation files (the "Software"), to deal
 *                 in the Software without restriction, including without limitation the rights
 *                 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *                 copies of the Software, and to permit persons to whom the Software is
 *                 furnished to do so, subject to the following conditions:
 *
 *                 The above copyright notice and this permission notice shall be included in all
 *                 copies or substantial portions of the Software.
 *
 *                 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *                 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *                 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *                 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *                 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *                 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *                 SOFTWARE.
 *
 * @comaptibility: The application was built and tested on Ubuntu 22.04. In the GitHub repository the latest DEB install package is
 *                 available for download.
 *
 * @author         Sorin Tudose
 *
 * @brief          This is a simple C++ HTTP server implementation using the Linux Socket API. The main feature is the ability to upload and download
 *                 files using HTTP requests. Consequently, the data fed into the server is stored locally on my personal computer in a MySQL database
 *                 using the MySQL cpp connector library.
 *
 *                 (https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-getting-started-examples.html)
 *
 * @github:        https://github.com/sorin373/HTTP-Server
 *
 * @testit:        In order to successfully run the server on your local machine you first need to setup a database. I uploaded the table formats int GitHub repository
 *                 as CSV files which can be used to get the app running quickly. Moreover, keep in mind that the executable must be run with SUDO as it needs
 *                 extra permissions to write / read the application files.
 *
 *                 Thank you for checking out my project!
 *
 */

#include "httpServer.hpp"
#include "client/clientWindow.hpp"

// #include <QApplication>

#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>

using namespace net;

int main(int argc, char *argv[])
{

    INIT(argc, argv);
    // QApplication app(argc, argv);

    // ui::Client client;
    // client.showMaximized();
    // client.show();

    // return app.exec();
}