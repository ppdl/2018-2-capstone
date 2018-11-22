obj-m += put-delay.o
put-delay-objs := put_delay.o file_ops.o
KDIR = /lib/modules/$(shell uname -r)/build
all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm -rf *.o *.ko *.mod.* .c* .t*
