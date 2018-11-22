KERNELDIR = /lib/modues/${shell uname -r}/build
obj-m := test.o

KDIR := /lib/modules/${shell uname -r}/build
PWD := ${shell pwd}

default:
	${MAKE} -C $(KDIR) SUBDIRS=$(PWD) CFLAGS+=-DEXPORT_SYMTAB modules
clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o
	rm -rf *.order
	rm -rf *.symvers
