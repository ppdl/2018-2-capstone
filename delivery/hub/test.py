from time import sleep

count=1
while True:
    sleep(0.1)
    print("Node1: {0:<6}, Node2: {1:<6}, Node3: {0:<6}\r".format(count, count*2, count*3), end='')
    count+=1
