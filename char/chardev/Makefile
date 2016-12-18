ifneq ($(KERNELRELEASE),)
obj-m:=memdev.o
else
KDIR=/lib/modules/`uname -r`/build
PWD=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.cmd *.o *.ko *.unsigned *.order *.mod.c .*.cmd .tmp_versions
endif
