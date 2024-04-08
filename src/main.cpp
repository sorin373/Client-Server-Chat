#include "httpServer.hpp"

int main(int argc, char *argv[])
{
    return net::INIT(argc, argv, AF_INET, SOCK_STREAM, IPPROTO_TCP);
}