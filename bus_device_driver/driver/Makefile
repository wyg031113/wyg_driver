ifneq ($(KERNELRELEASE),)
	obj-m:=worlddriver.o
else
	KDIR=/lib/modules/`uname -r`/build
	PWD=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make *.o *.order *.unsigned *.ko *.mod.c
endif
