/*************************************************************************
    > File Name: kern_task.c
    > Author: wyg
    > Mail: wyg_0802@126.com 
    > Created Time: 2016年12月14日 星期三 16时09分05秒
 ************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#define debug(fmt, ...) printk(KERN_DEBUG fmt,## __VA_ARGS__)
volatile static int stop = 0;
static void timer_handler(unsigned long data);
DEFINE_TIMER(test_timer, timer_handler, 3*HZ, (unsigned long)"I am a timer.");
static void timer_handler(unsigned long data)
{
	printk("Timer running..\n%s\n", (char *)data);
	test_timer.expires = jiffies + 5*HZ;
	add_timer(&test_timer);
}
static void tasklet_handler(unsigned long data)
{
	printk("Tasklet running..\n%s\n", (char *)data);
}
DECLARE_TASKLET(test_tasklet, tasklet_handler,(unsigned long) "I am a small task.");
static struct task_struct *kth;
static struct workqueue_struct *work_queue;
static void worker(struct work_struct *data)
{
	debug("this worker's work is %p\n", data);
	kfree(data);
}
static int kern_thread(void *data)
{
	struct work_struct *ws ;
	debug("kern_thread:data=%s\n", (char*)data);
	while(!stop){
		ws = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
		if(ws != NULL){
			INIT_WORK(ws, worker);
			queue_work(work_queue, ws);
		}else{
			debug("kmalloc struct work_struct failed.\n");
		}	
		tasklet_schedule(&test_tasklet);
		ssleep(3);
		debug("kern_thread running..\n");
	}
	debug("kern_thread_stopped.\n");
	return 0;
}
static __init int kern_task_init(void)
{
	debug("kern_task init.\n");
	work_queue = create_workqueue("hello_work_queue");
	if(work_queue == NULL){
		debug("create_workqueue  failed.\n");
		goto failed2;
	}
	kth = kthread_run(kern_thread, (void*)"Hello, I am a kernel thread.","test kthread%d", 0);
	if(IS_ERR(kth)){
		debug("kthread run failed.\n");
		goto failed1;
	}

	add_timer(&test_timer);
	return 0;
failed1:
	destroy_workqueue(work_queue);
failed2:
	return -1;
}

static __exit void kern_task_exit(void)
{
	debug("kern_task exit.\n");
	del_timer_sync(&test_timer);
	stop = 1;
	kthread_stop(kth);
	flush_workqueue(work_queue);
	destroy_workqueue(work_queue);
	tasklet_kill(&test_tasklet);
}

module_init(kern_task_init);
module_exit(kern_task_exit);
MODULE_LICENSE("GPL");
