ifneq ($(KERNELRELEASE),)
	obj-m:=memdev.o
else
	KDIR=/lib/modules/`uname -r`/build
	PWD=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.ko .*.cmd *.mod.c *.unsigned .tmp_versions
endif
