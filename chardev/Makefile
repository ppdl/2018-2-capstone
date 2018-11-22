obj-m := test.o
#read-test-objs := read_mod.o file_ops_cpy.o 

KDIR = /lib/modules/$(shell uname -r)/build
all :
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

