import socket

host = '172.20.10.3'
port = 9999


client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((host, port))

print("complete")

try:
    while True:
        #data = client_socket.recv(1024)
        #print(data.decode("utf-8"), len(data))

        s = input()
        client_socket.sendall(bytes(s, 'UTF-8'))
except Exception as e:
    print(e)

#client_socket.close()