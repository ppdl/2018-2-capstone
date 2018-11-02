import sys
import time
import logging
from watchdog.observers import Observer
from watchdog.events import FileSystemEventHandler 
import socket

HOST='10.42.0.1'
PORT=54321

class MyEventHandler(FileSystemEventHandler):
    def do_action(self, event):
        f = open("/proc/TBI/tbi", "r")
        line = f.readline()
        print(line)
        f.close()

    def on_any_event(self, event):
        self.do_action(event)


if __name__ == "__main__":
    path = sys.argv[1] if len(sys.argv) > 1 else '.'
    event_handler = MyEventHandler()
    observer = Observer()
    observer.schedule(event_handler, "/proc/TBI/", recursive=False)
    observer.start()
    try:
        while True:
            time.sleep(0.01)
    except KeyboardInterrupt:
        observer.stop()
    observer.join()
