import subprocess

def coll():
    proc1 = subprocess.Popen(['netstat', '-st'], stdout=subprocess.PIPE)
    proc2 = subprocess.Popen(['grep', 'retransmitted'], stdin=proc1.stdout,
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    proc1.stdout.close() # Allow proc1 to receive a SIGPIPE if proc2 exits.
    out, err = proc2.communicate()

    retransmitted = out.decode().split()[0]
    return retransmitted

if __name__ == '__main__':
    print(coll())
