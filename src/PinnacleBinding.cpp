#include "PinnacleAll.hpp"

extern "C"
{
    int init_server(void)
    {
        if (!net::server::__INIT__()) return -1;
        return 0;
    }

    int init_client(void)
    {
        if (!net::client::__INIT__()) return -1;
        return 0;
    }
}