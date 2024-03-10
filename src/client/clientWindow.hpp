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
            void __startServer__(void) noexcept;
            void __stopServer__(void) noexcept;

            QPushButton *startButton;
            QPushButton *stopButton;
            QTextEdit   *textDisplay;
            QLabel      *ipAddressLabel;
            QLabel      *portLabel;
        };
    };
};

#endif