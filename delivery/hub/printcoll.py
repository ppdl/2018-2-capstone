import socket
import time

TERMINAL0 = '10.42.0.223'
TERMINAL1 = '10.42.0.217'
TERMINAL2 = '10.42.0.190'
PORT      = 54210

coll_init_val = {
        TERMINAL0: 0,
        TERMINAL1: 0,
        TERMINAL2: 0,
        }
collision = {
        TERMINAL0: 0,
        TERMINAL1: 0,
        TERMINAL2: 0,
        }
try:
    sync=False
    while True:
        time.sleep(0.1)
        for HOST in [TERMIANL0, TERMINAL1, TERMINAL2]:
            with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                s.connect((HOST,PORT))
                s.sendall(b'a')

                coll=int(s.recv(1024).decode())
                if not sync:
                    coll_init_val[HOST]=coll
                else:
                    collision[HOST]=coll-coll_init_val[HOST]

        sync=True
        print("Node1: {0:<7}, Node2: {1:<7}, Node3: {2:<7}\r", end='')

except KeyboardInterrupt:
    
