ifneq ($(KERNELRELEASE),)
	obj-m:=wygbus.o
else
	KDIR=/lib/modules/`uname -r`/build
	PWD=$(shell pwd)
wygbus.ko:wygbus.c
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.ko *.mod.c *.unsigned *.order .tmp_versions
endif
