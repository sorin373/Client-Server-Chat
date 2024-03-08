#ifndef __CLIENT_WINDOW__
#define __CLIENT_WINDOW__

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>

namespace net
{
    namespace ui
    {
        class Client : public QWidget
        {
        public:
            explicit Client(QWidget *parent = nullptr);
            ~Client();

        private slots:
            void startServer(void);
            void stopServer(void);

        private:
            //void createMenu(void);
            void startButtonHandler(void);

            QLabel *ipAddressLabel;
            QLabel *portLabel;
            QPushButton *startButton;
            QPushButton *stopButton;
            QTextEdit *textDisplay;
        };
    };
};

#endif