ifneq ($(KERNELRELEASE),)
	obj-m:=wygdriver.o
else
	KDIR=/lib/modules/`uname -r`/build
	PWD=$(shell pwd)
wygdriver.ko:wygdriver.c
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.ko *.mod.c *.unsigned *.order
endif
