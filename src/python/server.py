import os
import ctypes

current_directory = os.path.dirname(os.path.abspath(__file__))
lib = ctypes.CDLL(os.path.join(current_directory, "libpinnacle.so"))

init_server = lib.init_server
init_server.argtypes = []
init_server.restype = ctypes.c_int

result_server = init_server()

if result_server == 0:
    print("Server initialization successful")
else:
    error_number = ctypes.get_errno()
    print(f"Server initialization failed with error code: {error_number}")