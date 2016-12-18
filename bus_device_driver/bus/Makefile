#ifndef ($(KERNELRELEASE),)
obj-m:=hellobus.o
#else
KDIR=/lib/modules/`uname -r`/build
PWD=$(shell pwd)
hellobus.ko:hellobus.c
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm *.o *.ko *.unsigned *.mod.c *.order
#endif
