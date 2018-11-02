obj-m := iotyr-terminal.o
iotyr-terminal-objs := terminal.o collision.o

KDIR = /lib/modules/$(shell uname -r)/build
all :
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

