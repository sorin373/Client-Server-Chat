import os
import ctypes

current_directory = os.path.dirname(os.path.abspath(__file__))

lib = ctypes.CDLL(os.path.join(current_directory, "libserver.so"))

init_server_c = lib.__INIT_SERVER_C__
init_server_c.restype = ctypes.c_int
result = init_server_c()

if result == 0:
    print("Server initialization successful")
else:
    print("Server initialization failed")