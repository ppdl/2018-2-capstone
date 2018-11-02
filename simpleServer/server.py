import socket
import time

HOST=''
PORT=54321

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    print("listening")
    s.bind((HOST,PORT))
    while True:
        s.listen()

        conn,addr=s.accept()
        with conn:
            print('Connected by', addr)
            while True:
                data = conn.recv(1024)
                print("length: " + str(len(data)))
                if not data:
                    break
            print('Completed\n')

