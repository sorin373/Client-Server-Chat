#include "serverUtils.hpp"

extern "C"
{
    int server(void)
    {
        if (!net::server::__INIT__())
            return -1;
        return 0;
    }
}