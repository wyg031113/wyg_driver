/*************************************************************************
    > File Name: hellobus.c
    > Author: wyg
    > Mail: wyg_0802@126.com 
    > Created Time: 2016年12月09日 星期五 08时09分48秒
 ************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>

static int hello_bus_match(struct device *dev, struct device_driver *drv)
{
	if(dev->kobj.name == NULL){
		printk(KERN_EMERG"device name is null.\n");
		return 0;
	}
	if(drv->name == NULL){
		printk(KERN_EMERG"driver name is null.\n");
		return 0;
	}
	return 0 == strcmp(dev->kobj.name, drv->name);
}
struct bus_type hello_bus_type = 
{
	.name		=	"hello_bus",
	.match		=	hello_bus_match
};
EXPORT_SYMBOL(hello_bus_type);
static void hello_bus_dev_release(struct device *dev)
{
	printk(KERN_EMERG"hello bus dev released.");
}
struct device hello_bus_dev = 
{
	.init_name		=	"hello_bus_dev",
	.release	=	hello_bus_dev_release,
};
EXPORT_SYMBOL(hello_bus_dev);

char *ver = "v4.0";
static int hello_bus_ver_show(struct bus_type *bus, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "hello_buse:%s\n", ver);
}
BUS_ATTR(ver, S_IRUGO, hello_bus_ver_show, NULL);
static int __init hello_bus_init(void)
{
	int status = 0;
	printk(KERN_EMERG"hello_bus inited.\n");
	
	if((status = device_register(&hello_bus_dev)) < 0){
		printk(KERN_EMERG"hello bus device reigster failed.\n");
		goto failed2;
	}
	if((status = bus_register(&hello_bus_type)) < 0){
		printk(KERN_EMERG"register bus failed!\n");
		goto failed1;
	}
	if((status = bus_create_file(&hello_bus_type, &bus_attr_ver))){
		printk(KERN_EMERG"register bus attr ver failed!\n");
		goto failed0;
	}
	return 0;
failed0:
	bus_unregister(&hello_bus_type);
failed1:
	device_unregister(&hello_bus_dev);
failed2:
	return status;

}

static void __exit hello_bus_exit(void)
{
	bus_unregister(&hello_bus_type);
	device_unregister(&hello_bus_dev);
	printk(KERN_EMERG"hello_bus exited.");
}

module_init(hello_bus_init);
module_exit(hello_bus_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("WYG");
MODULE_DESCRIPTION("hello bus test.");
module_param(ver, charp, S_IRUGO);
