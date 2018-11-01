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
#        last = f.readlines()[-1]
        print(line)
        f.close()

    def on_modified(self, event):
        print("Files are modified")
        self.do_action(event)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO,
                        format='%(asctime)s - %(message)s',
                        datefmt='%Y-%m-%d %H:%M:%S')
    path = sys.argv[1] if len(sys.argv) > 1 else '.'
    event_handler = MyEventHandler()
    observer = Observer()
    observer.schedule(event_handler, path, recursive=True)
    observer.start()
    try:
        pass
    except KeyboardInterrupt:
        observer.stop()
    observer.join()
