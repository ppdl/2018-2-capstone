import socket
import time

HOST=''
PORT=56789
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    print("listening for delay")
    s.bind((HOST,PORT))
    s.listen()
    while True:
        conn,addr=s.accept()
        with conn:
            delay=conn.recv(1024).decode()
            with open('delay','w') as f:
                f.write(delay)
