/*************************************************************************
    > File Name: worlddevice.c
    > Author: wyg
    > Mail: wyg_0802@126.com 
    > Created Time: 2016年12月09日 星期五 08时31分01秒
 ************************************************************************/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
extern struct bus_type hello_bus_type;
extern struct device hello_bus_dev;
static void world_device_release(struct device *dev)
{
	printk(KERN_EMERG"world device released.\n");
}
static struct device world_device = 
{
	.parent			=	&hello_bus_dev,
	.bus			=	&hello_bus_type,
	.init_name		=	"world_dev",
	.release		=	world_device_release
};
static int world_dev_size = 1024;
static ssize_t world_device_size_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "world_dev_size:%d\n", world_dev_size);
}
DEVICE_ATTR(size, S_IRUGO, world_device_size_show, NULL);
static int __init world_device_init(void)
{
	int status = 0;
	printk(KERN_EMERG"world device init.\n");
	if((status = device_register(&world_device)) < 0){
		printk(KERN_EMERG"register world device failed.\n");
		goto failed2;
	}
	if((status = device_create_file(&world_device, &dev_attr_size))){
		printk(KERN_EMERG"create world device file size failed.\n");
		goto failed1;
	}
	return 0;
failed1:
	device_unregister(&world_device);
failed2:
	return status;
}

static void __exit world_device_exit(void)
{
	device_unregister(&world_device);
	printk(KERN_EMERG"world device exited.\n");
}

module_init(world_device_init);
module_exit(world_device_exit);
module_param(world_dev_size, int, S_IRUGO);
MODULE_LICENSE("GPL");
