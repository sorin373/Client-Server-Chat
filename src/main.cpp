#include "app/ws_app.hpp"

int main(int argc, char **argv)
{
    int port = ws_app::get_port(argc, argv);

    if (port == -1)
        return -1;

    ws_app::core c(LOCALHOST, port);
    
    if (c.app_easy_start() == -1)
        return -1;

    return 0;
}