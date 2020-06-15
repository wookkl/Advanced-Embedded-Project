import socket
import threading
import time
import cv2
import numpy as np
import pytesseract
from PIL import Image

host = '172.20.10.2'
port = 9999
cap = cv2.VideoCapture(0)

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
server_socket.bind((host,port))
server_socket.listen(2)

def sendMsgToAchro(client_socket,message):
    print("sendToAchro")
    client_socket.sendall(bytes(message,'UTF-8'))

def sendMsgToAndroid(client_socket,message):
    print(message)
    client_socket.sendall(bytes(message+"\n",'UTF-8'))

def startCam():
    print("Cam Start")
    while True:
        ret,frame = cap.read()
        cv2.imshow('test',frame)
        
        key = cv2.waitKey(1)
        if key == 27:
            break
    cap.release()
    cv2.destroyAllWindows()

def receive(x):
    print("Receive Start")
    i = 1
    print(x[0][1][0]) #Achro -> addr -> ip
    print(x[1][1][0]) #Phone -> addr -> ip
    
    achro_socket = x[0][0]
    android_socket = x[1][0]
    
    while True:
        data = achro_socket.recv(1024)
        data = data[:len(data)-1]

        msg = data.decode("utf-8")
        if msg == "exit":
            print("exit")
            break;
        elif msg == "start":
            print("screenShot!!!")
            ret, frame = cap.read()
            cv2.imwrite('carNumber' + str(i) + '.jpg',frame)
            #car = cv2.imread('testimg3.jpg',cv2.IMREAD_COLOR)
            car = cv2.imread('carNumber' + str(i) + '.jpg',cv2.IMREAD_COLOR)
            n = imageProcessing(car)
            n += str(chr(i + 64))
            i += 1
            sendMsgToAndroid(android_socket, n)
        else:
            print(msg,len(msg))
    android_socket.close()
    achro_socket.close()


def imageProcessing(frame):
    copy_img = frame.copy()
    img2 = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    blur = cv2.GaussianBlur(img2,(3,3),0)
    canny = cv2.Canny(blur,100,200)
    cnts, contours, hierarchy = cv2.findContours(canny, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
    box1 = []
    f_count = 0
    select = 0
    plate_width = 0

    for i in range(len(contours)):
        cnt = contours[i]
        area = cv2.contourArea(cnt)
        x,y,w,h = cv2.boundingRect(cnt)
        rect_area = w * h
        aspect_ratio = float(w) / h

        if(aspect_ratio >= 0.2) and (aspect_ratio <= 1.0) and (rect_area >= 100) and (rect_area <= 700):
            cv2.rectangle(frame,(x,y),(x + w, y + h),(0,255,0),1)
            box1.append(cv2.boundingRect(cnt))
    for i in range(len(box1)):
        for j in range(len(box1) - (i + 1)):
            if box1[j][0] > box1[j+1][0]:
                temp = box1[j]
                box1[j] = box1[j+1]
                box1[j+1] = temp

    for m in range(len(box1)):
        count = 0
        for n in range(m+1,(len(box1)-1)):
            delta_x = abs(box1[n+1][0] - box1[m][0])

            if delta_x > 150:
                break
            delta_y = abs(box1[n+1][1] - box1[m][1])
            if delta_x == 0:
                delta_x = 1
            if delta_y == 0:
                delta_y = 1
            gradient = float(delta_y) / float(delta_x)
            if gradient < 0.25:
                count += 1
        if count > f_count:
            select = m
            f_count = count
            plate_width = delta_x
    
    number_plate = copy_img[box1[select][1]-10:box1[select][3]+box1[select][1]+20,box1[select][0]-10:140+box1[select][0]]
    resize_plate = cv2.resize(number_plate,None,fx=1.8,fy=1.8,interpolation=cv2.INTER_CUBIC+cv2.INTER_LINEAR)
    plate_gray = cv2.cvtColor(resize_plate,cv2.COLOR_BGR2GRAY)
    ret, th_plate = cv2.threshold(plate_gray,150,255,cv2.THRESH_BINARY)

    kernel = np.ones((3,3),np.uint8)
    er_plate = cv2.erode(th_plate,kernel,iterations = 1)
    er_invplate = er_plate
    cv2.imwrite('er_plate.jpg',er_invplate)
    result = pytesseract.image_to_string(Image.open('er_plate.jpg'))
    print(result.replace(" ",""))
    return result[4:]

def connect():
    isConnectedPhone = False
    isConnectedAchro = False
    clientInfo = [0 for i in range(2)]

    while True:
        print("wait...")
        client_socket, addr = server_socket.accept()
        if addr[0] == '172.20.10.5':
            print("--------Achro connect--------")
            clientInfo[0] = (client_socket, addr)
            isConnectedAchro = True
        elif addr[0] == '172.20.10.4':
            print("--------Android connect--------")
            clientInfo[1] = (client_socket, addr)
            isConnectedPhone = True

        if isConnectedAchro and isConnectedPhone:
            break
        else:
            print("connected by...", addr)
    print("----------------------Start camThread and recvThread-----------------------")
    camThread = threading.Thread(target = startCam)
    camThread.start()
    recvThread = threading.Thread(target = receive, args =[clientInfo])
    recvThread.start()
    
    
if __name__ == "__main__":
    connect()


