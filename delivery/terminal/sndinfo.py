import socket
import time

HOST=''
PORT=54210
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    print("listening for collision request")
    s.bind((HOST,PORT))
    while True:
        s.listen()

        conn,addr=s.accept()
        with conn:
            with open('/proc/collision_count', 'r') as f:
                collision = f.readline()
                conn.sendall(collision.encode())
                print('send: ' + collision)
