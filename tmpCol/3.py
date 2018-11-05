import subprocess

proc1 = subprocess.Popen(['netstat', '-st'], stdout=subprocess.PIPE)
proc2 = subprocess.Popen(['grep', 'retransmited'], stdin=proc1.stdout,
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE)

proc1.stdout.close() # Allow proc1 to receive a SIGPIPE if proc2 exits.
out, err = proc2.communicate()

retransmited = out.decode().split()[0]
print(retransmited)
