ifneq ($(KERNELRELEASE),)
	obj-m:=kern_task.o
else
	KDIR=/lib/modules/`uname -r`/build
	PWD=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.o *.ko *.unsigned .*.cmd *.order .tmp_versions *.mod.c
endif
