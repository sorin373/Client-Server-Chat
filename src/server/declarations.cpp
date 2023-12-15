#include "declarations.hpp"
#include "interface/interface.hpp"

net::server *__server = nullptr;
volatile int userCredentialsCount = 0;
char tableName[] = "user";