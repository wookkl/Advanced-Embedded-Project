import socket
import time

host = '192.168.123.39'
port = 9999 
 

server_sock = socket.socket(socket.AF_INET)
server_sock.bind((host, port))
server_sock.listen(1)

print("기다리는 중")
client_sock, addr = server_sock.accept()
print('Connected by', addr)

try:
    
    data = client_sock.recv(1024)
    print(data.decode("utf-8"), len(data))


    client_sock.sendall(bytes("나는 서버야\n",'UTF-8'))
    print("send complete")


    client_sock.close()
    server_sock.close()
except Exception as e:
    print(e)
