import os
import ctypes

# Load the shared library
current_directory = os.path.dirname(os.path.abspath(__file__))
lib = ctypes.CDLL(os.path.join(current_directory, "libpinnacle.so"))

init_client = lib.init_client
init_client.argtypes = []
init_client.restype = ctypes.c_int

result_client = init_client()

if result_client == 0:
    print("Client initialization successful")
else:
    error_number = ctypes.get_errno()
    print(f"Client initialization failed with error code: {error_number}")
