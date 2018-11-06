import socket
import time
from coll import coll

HOST=''
PORT=54210
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    print("listening for collision request")
    s.bind((HOST,PORT))
    s.listen()
    while True:

        conn,addr=s.accept()
        with conn:
            conn.sendall(coll().encode())
