ifneq ($(KERNELRELEASE),)
	obj-m:=worlddevice.o
else
	KDIR=/lib/modules/`uname -r`/build
	PWD=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.ko *.order *.unsigned *.mod.c
endif
