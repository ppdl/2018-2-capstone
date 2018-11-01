obj-m := netf-proc.o
netf-proc-objs :=  hello.o file_ops.o

KDIR = /lib/modules/$(shell uname -r)/build

all :
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

