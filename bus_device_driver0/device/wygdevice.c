/*************************************************************************
    > File Name: wygbus.c
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: 2016年12月08日 星期四 08时23分07秒
 ************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/string.h>
extern struct bus_type wyg_bus_type;
extern struct device wyg_bus_device;
static char *ver = "v1.0";
static ssize_t show_wygdev(struct device *bus, struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "wygdev:%s\n", ver);
}
DEVICE_ATTR(wygdev, S_IRUGO, show_wygdev, NULL);
static void wyg_dev_release(struct device *dev)
{
	printk(KERN_INFO"wyg device:%s released.\n", dev->kobj.name);
}
struct device wyg_device = 
{
	.init_name		=	"devhello",
	.bus			=	&wyg_bus_type,
	.parent			=	&wyg_bus_device,
	.release		=	wyg_dev_release
};
static __init int device_init(void)
{
	int status = 0;
	printk(KERN_EMERG"Device inited.\n");

	status = device_register(&wyg_device);
	if(status < 0)
		goto failed;
	status = device_create_file(&wyg_device, &dev_attr_wygdev);
	if(status < 0)
		goto failed1;
	return 0;
failed1:
	device_unregister(&wyg_device);
failed:
	return status;
}

static __exit void device_exit(void)
{
	device_unregister(&wyg_device);
	printk(KERN_CRIT"Device exit.\n");
}

module_init(device_init);
module_exit(device_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("WYG");
module_param(ver, charp, S_IRUGO);

