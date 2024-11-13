#pragma once

#define MAX_L 32
#define MAX_USERNAME_LENGTH MAX_L
#define MAX_PASSWORD_LENGTH 64

#define MAX_BUFFER_SIZE 4096

#define ROOT "interface"
#define DEFAULT_ROUTE "interface/login.html"

#ifndef USE_SSL
    #define USE_SSL false
#endif

#define LOCALHOST "0.0.0.0"
#define DEFAULT_PORT 8080

#ifdef _WIN32
    #define INVALID_SOCKET (SOCKET)(~0)
#else
    #define INVALID_SOCKET -1
#endif

#define CONNECTION_TIMEOUT 300
#define MAX_RPM 100

namespace net
{
    typedef unsigned char BYTE;
    typedef int SOCKET;

    enum class HTTP_STATUS 
    {
        SUCCESS              = 200,
        BAD_REQUEST          = 400,
        UNAUTHORIZED         = 401,
        FORBIDDEN            = 403,
        NOT_FOUND            = 404,
        SERVER_ERROR         = 500,
        SERVICE_UNAVAILABLE  = 503
    };
}