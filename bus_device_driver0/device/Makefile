ifneq ($(KERNELRELEASE),)
	obj-m:=wygdevice.o
else
	KDIR=/lib/modules/`uname -r`/build
	PWD=$(shell pwd)
wygdevice.ko:wygdevice.c
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.ko *.mod.c *.unsigned *.order
endif
