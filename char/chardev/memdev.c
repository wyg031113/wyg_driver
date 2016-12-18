/*************************************************************************
    > File Name: memdev.c
    > Author: wyg
    > Mail: wyg_0802@126.com 
    > Created Time: 2016年12月10日 星期六 09时24分51秒
 ************************************************************************/
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/completion.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#define MEMDEV_MAJOR 0
#define IRQ 3
#define BASE 0x2f8
#define THR (BASE)
#define IER (BASE+1)
#define FCR (BASE+2)
#define IIR (BASE+2)
#define LSR (BASE+5)
#define MAX_DATA_SIZE 4096
#define FIFO_SIZE 16
#include "memdev.h"
DECLARE_COMPLETION(work_send);
DECLARE_COMPLETION(work_recv);
DECLARE_MUTEX(sem);
spinlock_t splock;
static inline void open_send_intr(void)
{
	outb(0x3, IER);
	outb(0x08, BASE+4);
}

static inline void close_send_intr(void)
{
	outb(0x1, IER);
	outb(0x08, BASE+4);
}
/*
struct data_packet
{
	struct list_head node;
	char *data;
	int start;
	int end;
};
LIST_HEAD(send_list);
LIST_HEAD(recv_list);
static struct data_packet *alloc_data_packet(size_t size)
{
	struct data_packet *dp;
	dp = kmalloc(sizeof(struct data_packet));
	if(dp == NULL){
		printk(KERN_DEBUG"alloc data packet failed.\n");
		return NULL;
	}
	dp->data = kmalloc(size);
	if(dp->data == NULL){
		kfree(dp);
		printk(KERN_DEBUG"alloc data packet's data failed.\n");
		return NULL;
	}
	dp->start = dp->end = 0;
	return dp;
}
static struct data_packet *free_data_packet(struct data_packet *p)
{
	free(p->data);
	free(p);
}
*/
struct data_pkt
{
	char data[MAX_DATA_SIZE];
	int start;
	int end;
};
static struct data_pkt recvd, sendd;
struct class *vir_class;
struct device *dev;
static int memdev_major = MEMDEV_MAJOR;
static int memdev_open(struct inode *ind, struct file *filp)
{
	char x = 'a';
	int i, ret;
	sendd.start = sendd.end = 0;
	recvd.start = recvd.end = 0;
	outb(0x27, FCR);
	printk(KERN_EMERG"memdev open.\n");
/*	ret = down_interruptible(&sem);
	if(ret < 0){
		return ret;
	}
	*/
	spin_lock(&splock);
	return 0;
}
static int memdev_release(struct inode *ind, struct file *filp)
{
	printk(KERN_EMERG"memdev release.\n");
	spin_unlock(&splock);
//	up(&sem);
	return 0;
}

static ssize_t memdev_read(struct file *flp, char __user *buf, size_t len, loff_t *pos)
{
	return 0;	
}

static ssize_t memdev_write(struct file *flp, const char  __user *buf, size_t len, loff_t *pos)
{
	int send_len = len < MAX_DATA_SIZE ? len : MAX_DATA_SIZE;
	int ret = 0;
	char c;
	if(send_len == 0)
		return 0;
	
	ret = copy_from_user(sendd.data, buf, send_len);
	if(ret != 0){
		printk(KERN_DEBUG"copy to user should success, but no failed.\n");
		send_len -= ret;
	}
	sendd.start = 0;
	sendd.end = send_len;

	open_send_intr();
	//for(c = 'A'; c <'A'+512; c++)
//		outb(c,THR);
	ret = wait_for_completion_interruptible(&work_send);
	if(ret < 0)
		return ret;
	else
		return send_len;
	
}
static struct file_operations memdev_fops = 
{
	.owner		=	THIS_MODULE,
	.open		=	memdev_open,
	.read		=	memdev_read,
	.write		=	memdev_write,
	.release	=	memdev_release
};
static irqreturn_t serial_handler(int irq, void *dev_id)
{
	while(inb(LSR)&1){
		printk("%c", inb(BASE));
	}	
	printk("\n");
	if(inb(IIR) & 0x20 || inb(LSR) & 0x20){
		if(sendd.start ==sendd.end){
			sendd.start = sendd.end = 0;
			complete(&work_send);
			close_send_intr();
			printk("work finished.\n");
		}else{
			int i;
			for(i = sendd.start; (i-sendd.start) < FIFO_SIZE && i < sendd.end; i++){
				outb(sendd.data[i], THR);
			}
			sendd.start = i;
			printk("sending.\n");
		}
	}
	return IRQ_HANDLED;
}
static struct cdev memdev;
static dev_t devno;
static int __init memdev_init(void)
{
	int ret = 0;
	int i;
	devno = MKDEV(memdev_major, 0);
	printk(KERN_EMERG"memdev init.\n");
	if(memdev_major){
		ret = register_chrdev_region(devno, 1, "memdev");
	}else{
		ret = alloc_chrdev_region(&devno, 0, 1, "memdev");
		memdev_major = MAJOR(devno);
	}
	if(ret < 0){
		printk(KERN_EMERG"register chardev number failed.\n");
		goto failed1;
	}else{
		printk(KERN_EMERG"regist chardev->major:%d, minor:%d\n", memdev_major, MINOR(devno));
	}
	cdev_init(&memdev, &memdev_fops);
	memdev.owner = THIS_MODULE;
	ret = cdev_add(&memdev, devno, 1);
	if(ret < 0){
		printk(KERN_EMERG"add cdev failed\n");
		goto failed2;
	}
	vir_class = class_create(THIS_MODULE, "virtual_device_class");
	if(IS_ERR(vir_class)){
		printk(KERN_EMERG"class create failed\n");
		goto failed3;
	}
	dev = device_create(vir_class, NULL, devno, &memdev, "memdev%d", 0);
	if(IS_ERR(dev)){
		printk(KERN_EMERG"dev create failed\n");
		goto failed4;
	}
	ret = request_irq(IRQ, serial_handler, IRQF_SHARED|IRQF_TRIGGER_RISING, "memdev", &memdev);
	if(ret < 0){
		printk("request irq memdev. failed\n");
		goto failed5;
	}
	request_region(BASE, 8, "memdev");
	outb(0x83, BASE+3);
	outb(0x0c, BASE);
	outb(0x00, BASE+1);
	outb(0x03, BASE+3);
	outb(0x01, IER);
	outb(0x27, FCR);
	outb(0x08, BASE+4);
	outb(0x00, BASE+6);
	for(i = 0; i < 8; i++){
		int d = inb(BASE+i);
		printk("reg%d:%x\n", i, d);
	}
	//enable_irq(IRQ);
	spin_lock_init(&splock);
	return 0;
failed5:
	device_destroy(vir_class, devno);
failed4:
	class_destroy(vir_class);
failed3:
	cdev_del(&memdev);
failed2:
	unregister_chrdev_region(devno, 1);
failed1:
	return ret;
}

static void __exit memdev_exit(void)
{
	printk(KERN_EMERG"memdev exit.\n");
	release_region(BASE, 8);
	free_irq(IRQ, &memdev);
	device_destroy(vir_class, devno);
	class_destroy(vir_class);
	cdev_del(&memdev);
	unregister_chrdev_region(devno, 1);
}

module_init(memdev_init);
module_exit(memdev_exit);
MODULE_LICENSE("GPL");
