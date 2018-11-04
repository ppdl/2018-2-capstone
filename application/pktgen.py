import random
import socket
import time

MEGA_BIG_PKT    = 200000000  #200M  5%
SUPER_BIG_PKT   = 15000000   #15M   5%
BIG_PKT         = 1000000    #1M    5%
NORM_PKT        = 50000      #50K   15%
SMALL_PKT       = 5000       #5K    20%
TINY_PKT        = 100        #100B  50%

MIN_SLEEP_TIME = 5
MAX_SLEEP_TIME = 10 

HOST='10.42.0.1'
PORT=54321

while True:
    randSleep = random.randrange(MIN_SLEEP_TIME, MAX_SLEEP_TIME)
    randSize = random.randrange(1,100)
    if randSize > 95:
        randStr = 'c' * random.randrange(1,MEGA_BIG_PKT)
    elif randSize > 90:
        randStr = 'c' * random.randrange(1,SUPER_BIG_PKT)
    elif randSize > 85:
        randStr = 'c' * random.randrange(1,BIG_PKT)
    elif randSize > 70:
        randStr = 'c' * random.randrange(1,NORM_PKT)
    elif randSize > 50:
        randStr = 'c' * random.randrange(1,SMALL_PKT)
    else:
        randStr = 'c' * random.randrange(1,TINY_PKT)

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.connect((HOST,PORT))
        print("data length: " + str(len(randStr)))
        s.sendall(randStr.encode())

    print("random sleep:", randSleep)
    time.sleep(randSleep)
