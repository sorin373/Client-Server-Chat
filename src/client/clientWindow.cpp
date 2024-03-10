// #include "clientWindow.hpp"
// #include "../server/serverUtils.hpp"
// #include "../server/global.hpp"

// #include <QVBoxLayout>
// #include <QPushButton>
// #include <QLabel>
// #include <QMenuBar>
// #include <QMenu>
// #include <QMessageBox>
// #include <QTextEdit>
// #include <qdebug.h>
// #include <thread>

// using namespace net::ui;

// Client::Client(QWidget *parent) : QWidget(parent)
// {
//     // Layout
//     QVBoxLayout *layout = new QVBoxLayout(this);
//     layout->setContentsMargins(20, 20, 20, 20);
//     layout->setSpacing(20);

//     // Buttons Layout
//     QHBoxLayout *buttonsLayout = new QHBoxLayout;
//     layout->addLayout(buttonsLayout);

//     // Start button
//     startButton = new QPushButton("Start Server", this);
//     startButton->setFixedSize(200, 100);
//     connect(startButton, &QPushButton::clicked, this, &Client::startServer);
//     startButton->setStyleSheet("background-color: white;"
//                                "padding: 5px;"
//                                "font-size: 25px;"
//                                "border-style: solid;"
//                                "border-color: black;"
//                                "border-width: 2px;"
//                                "border-radius: 10px;");
//     buttonsLayout->addWidget(startButton);

//     // Stop button
//     stopButton = new QPushButton("Stop Server", this);
//     stopButton->setFixedSize(200, 100);
//     stopButton->setEnabled(false); // Initially disabled
//     connect(stopButton, &QPushButton::clicked, this, &Client::stopServer);
//     stopButton->setStyleSheet("background-color: white;"
//                               "padding: 5px;"
//                               "font-size: 25px;"
//                               "border-style: solid;"
//                               "border-color: black;"
//                               "border-width: 2px;"
//                               "border-radius: 10px;");
//     buttonsLayout->addWidget(stopButton);

//     // IP and Port Layout
//     QVBoxLayout *infoLayout = new QVBoxLayout;
//     layout->addLayout(infoLayout);

//     // IP address label
//     ipAddressLabel = new QLabel("IP Address: ", this);
//     ipAddressLabel->setFixedSize(200, 50);
//     infoLayout->addWidget(ipAddressLabel);

//     // Port label
//     portLabel = new QLabel("Port: ", this);
//     portLabel->setFixedSize(200, 50);
//     infoLayout->addWidget(portLabel);

//     // Text Display
//     textDisplay = new QTextEdit(this);
//     textDisplay->setReadOnly(true);
//     layout->addWidget(textDisplay);
// }

// void Client::startServer()
// {
//     startButton->setEnabled(false);
//     stopButton->setEnabled(true);
//     textDisplay->append("Server starting...");
    
//     std::thread(&Client::__startServer__, this).detach();
// }

// void Client::__startServer__(void) noexcept
// {
//     net::INIT(0, nullptr);
// }

// void Client::stopServer()
// {
//     startButton->setEnabled(true);
//     stopButton->setEnabled(false);
//     textDisplay->append("Server shutting down...");

//     if (__server) {
//         // Safely stop the server if __server is valid
//         std::thread(&Client::__stopServer__, this).detach();
//     } else {
//         // Handle the case where __server is invalid
//         textDisplay->append("Error: Server object is invalid.");
//     }
// }

// void Client::__stopServer__() noexcept
// {
//     if (__server) {
//         // Call haltServer only if __server is valid
//         __server->haltServer();
//     }
// }

// Client::~Client()
// {
//     delete ipAddressLabel;
//     delete portLabel;
//     delete startButton;
//     delete stopButton;
//     delete textDisplay;
// }
