ifneq ($(KERNELRELEASE),)
obj-m:=wyg.o
wyg-objs:=compt.o
else
	KDIR=/lib/modules/`uname -r`/build
	PWD=$(shell pwd)
all:compt.c
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.ko .*.cmd *.mod.c *.order *.unsigned .tmp_versions
endif
