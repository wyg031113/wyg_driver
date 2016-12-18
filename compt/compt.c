/*************************************************************************
    > File Name: compt.c
    > Author: wyg
    > Mail: wyg_0802@126.com 
    > Created Time: 2016年12月15日 星期四 08时38分39秒
 ************************************************************************/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/completion.h>
#include <linux/semaphore.h>
#include <linux/spinlock.h>
#define debug(fmt, ...) printk(KERN_DEBUG fmt, ##__VA_ARGS__);
spinlock_t splock;
DECLARE_COMPLETION(comple);
struct semaphore sema;
static struct workqueue_struct *wkq;
static void worker(struct work_struct *ws);
volatile static int stop = 0;
static struct task_struct *ts1, *ts2, *ts3;
static int my_kthread(void *data)
{
	int id = (int)data;
	debug("Kthread created.\n");

	while(!stop){
		if(id == 1){
			struct work_struct *ws = kmalloc(sizeof(struct work_struct), GFP_KERNEL);
			debug("thread %d running.\n", id);
			if(ws != NULL){
				INIT_WORK(ws, worker);
				queue_work(wkq, ws);
				schedule_work(ws);
				complete(&comple);
			}else{
				debug("kmalloc struct work_struct failed.\n");
			}
			ssleep(5);
		}else if(id == 2){
			int ret = wait_for_completion_interruptible(&comple);
			debug("thread %d wake, ret=%d.\n", id, ret);
		}else if(id == 3){
			int ret;
			ret = down_interruptible(&sema);
			debug("thread %d wake, ret=%d\n", id, ret);
		}
	}
	return 0;
}
static void small_task(unsigned long data)
{
	debug("tasklet %ld running.\n", data);
	spin_lock(&splock);
	up(&sema);
	spin_unlock(&splock);
}
DECLARE_TASKLET(my_tasklet, small_task, 1);

static void my_timer(unsigned long data);
DEFINE_TIMER(timer1, my_timer, 0, 1);
static void my_timer(unsigned long data)
{
	debug("my_timer %ld running...\n", data);
	tasklet_hi_schedule(&my_tasklet);
	timer1.expires = jiffies + 4 * HZ;
	add_timer(&timer1);
}

static void worker(struct work_struct *ws)
{
	debug("worker running.\n");
	up(&sema);
	kfree(ws);
}
static int __init compt_init(void)
{
	debug("init.\n");
	spin_lock_init(&splock);
	sema_init(&sema, 0);
	wkq = create_workqueue("wyg_workqueue");
	if(wkq == NULL){
		return -ENOMEM;
	}
	ts1 = kthread_run(my_kthread, (void *)1, "my_kthread%d",1);
	ts2 = kthread_run(my_kthread, (void *)2, "my_kthread%d",2);
	ts3 = kthread_run(my_kthread, (void *)3, "my_kthread%d",2);
	add_timer(&timer1);
	
	return 0;
}

static void __exit compt_exit(void)
{
	stop = 1;
	up(&sema);
	complete(&comple);
	kthread_stop(ts3);
	kthread_stop(ts2);
	kthread_stop(ts1);
	del_timer(&timer1);
	tasklet_kill(&my_tasklet);
	flush_workqueue(wkq);
	destroy_workqueue(wkq);
	debug("exit.\n");
}

module_init(compt_init);
module_exit(compt_exit);
MODULE_LICENSE("GPL");
