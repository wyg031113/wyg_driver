KERN_DIR = /lib/modules/`uname -r`/build
all:
	make -C $(KERN_DIR) M=`pwd` modules
clean:
	rm -rf modules.order *.order *.ko *.o .*.cmd *.unsigned .tmp_versions *.mod.c
obj-m += myramblock.o
