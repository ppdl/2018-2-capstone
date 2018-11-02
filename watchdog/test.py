import time

while True:
    time.sleep(1)
    with open('/proc/tbi', 'r') as f:
        print(f.readline())
