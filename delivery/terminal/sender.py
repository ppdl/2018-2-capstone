import socket
import time
from collision_notify import current_collision

HOST='10.42.0.1'
PORT=54210
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST,PORT))
    while True:
        collision = current_collision()
        s.sendall(collision.encode())
