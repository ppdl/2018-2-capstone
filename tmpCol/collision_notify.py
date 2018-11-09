import subprocess

def current_collision():
    proc1 = subprocess.Popen(['netstat', '-st'], stdout=subprocess.PIPE)
    proc2 = subprocess.Popen(['grep', 'retransmited'], stdin=proc1.stdout,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    proc1.stdout.close() # Allow proc1 to receive a SIGPIPE if proc2 exits.
    out, err = proc2.communicate()

    collision = out.decode().split()[0]
    return collision

if __name__ == "__main__":
    print(current_collision())
