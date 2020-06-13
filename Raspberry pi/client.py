import socket

host = '192.168.0.185'
port = 9999


client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((host, port))

print("complete")
#client_socket.close()
