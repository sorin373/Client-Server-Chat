#include "serverUtils.hpp"

int main()
{
    if (!net::server::__INIT__())
        return -1;

    return 0;
}