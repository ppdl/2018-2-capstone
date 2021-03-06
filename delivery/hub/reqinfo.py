import socket
import time

TERMINAL0 = '10.42.0.223'
TERMINAL1 = '10.42.0.217'
TERMINAL2 = '10.42.0.190'
PORT      = 54210

COLLISION = {
        TERMINAL0: 0,
        TERMINAL1: 0,
        TERMINAL2: 0,
        }

try:
    with open('result.txt', 'w') as result:
        while True:
            time.sleep(5)
            with open("/proc/tbi", "r") as f:
                line = f.readline()
                print(line)
                for HOST in [TERMINAL0, TERMINAL1, TERMINAL2]:
                    COLLISION[HOST]=0
                    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                        print('request collision # to %s'%HOST)
                        s.connect((HOST,PORT))
                        s.sendall(b'a')

                        collision =  int(s.recv(1024).decode())
                        print('collision of %s: %d'%(HOST,collision))
                        if collision:
                            COLLISION[HOST] = collision
                if all(v for v in COLLISION.values()):  
                    line += ',%d,%d,%d\n'%(COLLISION[TERMINAL0], COLLISION[TERMINAL1], COLLISION[TERMINAL2])
                    print('write record: ' + line)
                    result.write(line)
except KeyboardInterrupt:
    f.close()



