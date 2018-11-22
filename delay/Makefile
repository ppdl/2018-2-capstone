obj-m += put-delay.o
put-delay-objs := put_delay.o file_ops.o
KDIR = /usr/src/linux-headers-4.15.0-38-generic
all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm -rf *.o *.ko *.mod.* .c* .t*
