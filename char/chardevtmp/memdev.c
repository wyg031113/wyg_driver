/*************************************************************************
    > File Name: memdev.c
    > Author: wyg
    > Mail: wyg_0802@126.com 
    > Created Time: 2016年12月10日 星期六 15时30分20秒
 ************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
static int memdev_major = 0;
static dev_t devno;
static struct class *vir_class;
static struct cdev memdev;
static struct device *memdev1;
static int memdev_open(struct inode *ind, struct file *flp)
{
	printk(KERN_DEBUG"memdev open.\n");
	return 0;
}

static int memdev_release(struct inode *ind, struct file *flp)
{
	printk(KERN_DEBUG"memdev failed.\n");
	return 0;
}
static ssize_t memdev_read(struct file *flp, char __user *buf, size_t count, loff_t *pos)
{
	printk(KERN_DEBUG"memdev read.\n");
	return count;
}
static ssize_t memdev_write(struct file *flp, const char __user *buf, size_t count, loff_t *pos)
{
	printk(KERN_DEBUG"memdev write.\n");
	return count;
}
static struct file_operations memdev_fops = 
{
	.owner		=	THIS_MODULE,
	.open		=	memdev_open,
	.read		=	memdev_read,
	.write		=	memdev_write,
	.release	=	memdev_release
};
static int __init memdev_init(void)
{
	int ret = 0;
	printk(KERN_DEBUG"memdev init.");
	if(memdev_major != 0){
		devno = MKDEV(memdev_major, 10);
		ret = register_chrdev_region(devno, 1, "memdev");
	}else{
		devno = 0;
		ret = alloc_chrdev_region(&devno, 20, 1, "memdev");
		memdev_major = MAJOR(devno);
	}
	if(ret < 0){
		printk(KERN_DEBUG"char dev register failed.\n");
		goto failed1;
	}else{
		printk(KERN_DEBUG"char dev:major:%d minor:%d\n", 
				MAJOR(devno), MINOR(devno));
	}
	
	cdev_init(&memdev, &memdev_fops);
	memdev.owner = THIS_MODULE;
	ret = cdev_add(&memdev, devno, 1);
	if(ret < 0){
		printk(KERN_DEBUG"cdev add failed.\n");
		goto failed2;
	}

	vir_class = class_create(THIS_MODULE, "vir_class");
	if(IS_ERR(vir_class)){
		printk(KERN_DEBUG"vir class create failed.\n");
		goto failed3;
	}
	
	memdev1 = device_create(vir_class, NULL, devno, NULL, "memdev%d", 1);
	if(IS_ERR(memdev1)){
		printk(KERN_DEBUG"device create failed.\n");
		goto failed4;
	}

	return 0;
failed4:
	class_destroy(vir_class);
failed3:
	cdev_del(&memdev);
failed2:
	unregister_chrdev_region(devno, 0);
failed1:
	return ret;
}

static void __exit memdev_exit(void)
{
	printk(KERN_DEBUG"memdev exit.");
	device_del(memdev1);
	class_destroy(vir_class);
	cdev_del(&memdev);
	unregister_chrdev_region(devno, 0);
}

module_init(memdev_init);
module_exit(memdev_exit);
MODULE_LICENSE("GPL");
