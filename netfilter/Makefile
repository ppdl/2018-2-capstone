obj-m := netf-proc.o
netf-proc-objs := hub.o tbi.o

KDIR = /lib/modules/$(shell uname -r)/build
all :
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

