#include "serverUtils.hpp"

extern "C"
{
    int __INIT_SERVER_C__(void)
    {
        if (!net::server::__INIT__())
            return -1;
        return 0;
    }
}