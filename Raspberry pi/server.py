import socket
import os
def Main():
    host='172.20.10.2'
    port=8000 
    s=socket.socket(socket.AF_INET,socket.SOCK_STREAM) #IPv4
    f = open('download.png','rb')
    size = os.path.getsize("download.png")
    
    s.bind((host,port)) 
    s.listen(5)
    conn,addr=s.accept()
    conn.send("Hello Server!!".encode('ascii'))
    '''
    conn.send(str(size).encode('ascii'))
    l= f.read(1024)
    
    print("Sending...")
    while l:
        print("Sending...")
        #s.send(l)
        l= f.read(1024)
    print('Done Sending')
    conn.send("Thank you for connection")
    f.close()
    '''
    conn.close()
    s.close()
if __name__=='__main__':
	Main()
