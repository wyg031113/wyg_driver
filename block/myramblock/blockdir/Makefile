KERN_DIR = /lib/modules/`uname -r`/build
all:
	make -C $(KERN_DIR) M=`pwd` modules
clean:
	rm -rf modules.order
obj-m += myramblock.o
