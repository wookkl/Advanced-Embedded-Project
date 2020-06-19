import socket

host = '192.168.0.185'
port = 9999


client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect((host, port))

print("complete")

while True:    
    s = input()
    client_socket.sendall(bytes(s + "\n", 'UTF-8'))
    msg = client_socket.recv(1024)
    if len(msg) >= 1:
        break


msg = msg.decode('utf-8')
print("car num :" ,msg)




msg = client_socket.recv(1024)
msg = msg.decode('utf-8')
print("charge by phone : ",msg)





#client_socket.close()
#192.168.0.106
