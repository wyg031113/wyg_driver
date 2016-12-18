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
static char *ver = "v1.0";
static ssize_t show_wygdrv(struct device_driver *drv, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "wygdrv:%s\n", ver);
}
DRIVER_ATTR(wygdrv, S_IRUGO, show_wygdrv, NULL);
static int wyg_drv_remove(struct device *dev)
{
	printk(KERN_INFO"wyg device driver released.\n");
	return 0;
}

static int wyg_probe(struct device *dev)
{
	printk(KERN_EMERG"wyg probe:driver found device.\n");
	return 0;
}
struct device_driver wyg_driver = 
{
	.name		=	"devhello",
	.bus		=	&wyg_bus_type,
	.probe		=	wyg_probe,
	.remove		=	wyg_drv_remove
};
static __init int wyg_driver_init(void)
{
	int status = 0;
	printk(KERN_EMERG"Driver inited.\n");

	status = driver_register(&wyg_driver);
	if(status < 0)
		goto failed;
	status = driver_create_file(&wyg_driver, &driver_attr_wygdrv);
	if(status < 0)
		goto failed1;
	return 0;
failed1:
	driver_unregister(&wyg_driver);
failed:
	return status;
}

static __exit void wyg_driver_exit(void)
{
	driver_unregister(&wyg_driver);
	printk(KERN_CRIT"Driver exit.\n");
}

module_init(wyg_driver_init);
module_exit(wyg_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("WYG");
module_param(ver, charp, S_IRUGO);

