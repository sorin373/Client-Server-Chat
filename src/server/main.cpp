#include "serverUtils.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    if (!net::server::__INIT__(argv[1]))
        return EXIT_FAILURE;

    return 0;
}