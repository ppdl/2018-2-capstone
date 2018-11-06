import socket

HOST=''
PORT=54321
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST,PORT))
    s.listen()
    while True:
        conn, addr = s.accept()
        received = 0
        while True:
            data = conn.recv(1024)
            received += len(data)
            if not data:
                break
        print("received %d from %s"%(received,addr[0]))
