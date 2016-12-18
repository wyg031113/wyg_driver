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
static char *ver = "v1.0";
static ssize_t show_wygbus(struct bus_type *bus, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "wygbus:%s\n", ver);
}
static int wygbus_match(struct device *dev, struct device_driver *driver)
{
	printk(KERN_INFO"wygbus_match:%p %p\n",dev->kobj.name, driver->name);
	if(!dev->kobj.name || !driver->name)
		return 0;
	return strcmp(dev->kobj.name, driver->name) == 0;
}
BUS_ATTR(wygbus, S_IRUGO, show_wygbus, NULL);
struct bus_type wyg_bus_type = 
{
	.name		=	"wygbus",
	.match		=	wygbus_match,
};
EXPORT_SYMBOL(wyg_bus_type);
static void wyg_bus_dev_release(struct device *dev)
{
	printk(KERN_INFO"wygbus device release.\n");
}
struct device wyg_bus_device = 
{
	.init_name		=	"wygbus0",
	.release	=	wyg_bus_dev_release
};
EXPORT_SYMBOL(wyg_bus_device);
static __init int bus_init(void)
{
	int status = 0;
	printk(KERN_EMERG"Bus inited.\n");
	status = bus_register(&wyg_bus_type);
	if(status < 0)
		goto failed;

	status = bus_create_file(&wyg_bus_type, &bus_attr_wygbus);
	if(status < 0)
		goto failed1;

	status = device_register(&wyg_bus_device);
	if(status < 0)
		goto failed1;
	return 0;

failed1:
	bus_unregister(&wyg_bus_type);
failed:
	return status;
}

static __exit void bus_exit(void)
{
	device_unregister(&wyg_bus_device);
	bus_unregister(&wyg_bus_type);
	printk(KERN_CRIT"Bus exit.\n");
}

module_init(bus_init);
module_exit(bus_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("WYG");
module_param(ver, charp, S_IRUGO);

