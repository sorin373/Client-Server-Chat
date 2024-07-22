#include "server/web_server.hpp"

int main(int argc, char *argv[])
{
    net::web_server ws("0.0.0.0", 8080);

    if (ws.ws_init(argc, argv) != 0)
        return -1;

    ws.run();

    return 0;
}