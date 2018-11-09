import socket

TERMINAL0 = '10.42.0.233'
TERMINAL1 = '10.42.0.217'
TERMINAL2 = '10.42.0.190'

HOST=''
PORT=54210

COLLISION = {
        TERMINAL0: 0,
        TERMINAL1: 0,
        TERMINAL2: 0,
        }

def update_collision():
    print('receiving collisions of each terminal...')
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST,PORT))
        s.listen()
        while True:
            conn, addr = s.accept()
            while True:
                data = conn.recv(1024)
                if not data:
                    break
                print('update %s to %d'%(addr[0],data.decode()))
                COLLISION[addr[0]] = data.decode()

if __name__ == '__main__':
    update_collision()
