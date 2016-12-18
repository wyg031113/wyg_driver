ifneq ($(KERNELRELEASE),)
	obj-m:=blockdev.o
else
	KDIR=/lib/modules/`uname -r`/build
	PWD=$(shell pwd)
all:blockdev.c
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.ko *.order *.unsigned .*.mod.c .tmp_versions *.mod.c
endif
