/*************************************************************************
    > File Name: worlddriver.c
    > Author: wyg
    > Mail: wyg_0802@126.com 
    > Created Time: 2016年12月09日 星期五 18时44分53秒
 ************************************************************************/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
extern struct bus_type hello_bus_type;
static int world_driver_probe(struct device *dev)
{
	printk(KERN_EMERG"driver found device.\n");
	return 0;
}
static int  world_driver_remove(struct device *dev)
{
	printk(KERN_EMERG"driver:device removed.\n");
	return 0;
}
struct device_driver world_driver = 
{
	.name		=	"world_dev",
	.probe		=	world_driver_probe,
	.bus		=	&hello_bus_type,
	.remove		=	world_driver_remove
};

static char *world_driver_name = "world_driver";
static ssize_t world_driver_name_show(struct device_driver *drv, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "driver:%s\n", world_driver_name);
}
DRIVER_ATTR(name, S_IRUGO, world_driver_name_show, NULL);
static int __init world_driver_init(void)
{
	int status = 0;
	if((status = driver_register(&world_driver)) < 0){
		printk("driver register failed.\n");
		goto failed1;
	}

	if((status = driver_create_file(&world_driver, &driver_attr_name)) < 0){
		printk("driver create file failed!\n");
		goto failed2;
	}
	printk(KERN_EMERG"world driver inited.\n");
	return 0;
failed2:
	driver_unregister(&world_driver);
failed1:
	return status;
}

static void __exit world_driver_exit(void)
{
	printk(KERN_EMERG"world driver exited.\n");
	driver_unregister(&world_driver);
}


module_init(world_driver_init);
module_exit(world_driver_exit);
module_param(world_driver_name, charp, S_IRUGO);
MODULE_LICENSE("GPL");
