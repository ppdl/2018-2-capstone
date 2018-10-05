obj-m += jprobe.o
KDIR = $(HOME)/linux
all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm -rf *.o *.ko *.mod.* .c* .t*
