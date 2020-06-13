import socket
import threading
import time
import cv2

host = '192.168.0.106'
port = 9999


'''
server_sock = socket.socket(socket.AF_INET)
server_sock.bind((host, port))
server_sock.listen(1)

print("기다리는 중")
client_sock, addr = server_sock.accept()
print('Connected by', addr)

try:

    s = input()
    client_sock.sendall(bytes(s + "\n", 'UTF-8'))
    print("send complete")

    
    data = client_sock.recv(1024)
    print(data.decode("utf-8"), len(data))


    client_sock.sendall(bytes("나는 서버야\n",'UTF-8'))
    print("send complete")
    
except Exception as e:
    print(e)

client_sock.close()
server_sock.close()
'''

def startCam():
    print("Cam Start")
    cap = cv2.VideoCapture(2)

    while True:
        ret,frame = cap.read()
        cv2.imshow('test',frame)

        k = cv2.waitKey(1)
        if key == 27:
            break
    cap.release()
    cv2.destroyAllWindows()

def receive():
    print("Receive Start")
    
def connect():
    isConnectedPhone = False
    isConnectedAchro = False
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((host,port))
    server_socket.listen(2)
    
    while True:
        print("wait...")
        client_socket, addr = server_socket.accept()
        
        if addr[0] == '192.168.0.185':
            print("Achro connect")
            isConnectedAchro = True
        elif addr[0] == '192.168.0.160':
            print("Phone connect")
            isConnectedPhone = True

        if isConnectedAchro and isConnectedPhone:
            break
        else:  
            print("connected by...", addr)
    print("Start camThread and recvThread")
    camThread = threading.Thread(target = startCam)
    camThread.start()
    recvThread = threading.Thread(target = receive)
    recvThread.start()
    
    
    client_socket.close()
    server_socket.close()
    
    #receive funtion and cam function
if __name__ == "__main__":
    connect()
