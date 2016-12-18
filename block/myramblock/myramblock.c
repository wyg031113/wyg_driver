#include <linux/module.h>
#include <linux/errno.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/ioport.h>
#include <linux/init.h>
#include <linux/wait.h>
#include <linux/blkdev.h>
#include <linux/blkpg.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/dma.h>

#define RAMBLOCK_SIZE (1024*1024)
static struct gendisk *my_ramblock_disk;
static struct request_queue *my_ramblock_queue;
static DEFINE_SPINLOCK(my_ramblock_lock);
static int major;
static unsigned char *my_ramblock_buf;
static int my_ramblock_getgeo(struct block_device *bdev, struct hd_geometry *geo)
{
 /* 容量=heads*cylinders*sectors*512 */
 geo->heads = 2; //磁头数
 geo->cylinders = 32; //柱面数
 geo->sectors = RAMBLOCK_SIZE/2/32/512; //扇区数
 return 0;
}

static void do_my_ramblock_request(struct request_queue *q)
{
 struct request *req;
 static int r_cnt=0; //实验用，打印出驱动读与写的调度方法
 static int w_cnt=0;
 
 req = blk_fetch_request(q);
 while (req)
  {
  unsigned long start = blk_rq_pos(req) *512;
  unsigned long len = blk_rq_cur_bytes(req);
  if(rq_data_dir(req)==READ)
   {
    memcpy(req->buffer, my_ramblock_buf+start, len); //读操作，
    printk("do_my_ramblock-request read %d times\n",r_cnt++);
   }
  else
   {
    memcpy( my_ramblock_buf+start,req->buffer, len); //写操作
    printk("do_my_ramblock request write %d times \n",w_cnt++);
   }
  
  // end_request(req,1);
    if(!__blk_end_request_cur(req, 0)) 
      {
             req = blk_fetch_request(q);
            }
  }
}

static const struct block_device_operations my_ramblock_fops =
{
 .owner = THIS_MODULE,
 .getgeo = my_ramblock_getgeo, //获取磁盘几何属性
};
 

static int my_ramblock_init(void)
{
 /*分配一个 gendisk 结构体*/
 my_ramblock_disk=alloc_disk(10);//次设备个数 ，分区个数 +1
 
 major=register_blkdev(0, "my_ramblock");
 
 //分配设置请求队列，提供读写能力
 my_ramblock_queue=blk_init_queue(do_my_ramblock_request,&my_ramblock_lock);
    //设置硬盘属性 
 my_ramblock_disk->major=major;
 my_ramblock_disk->first_minor=0;
 my_ramblock_disk->fops=&my_ramblock_fops;
 sprintf(my_ramblock_disk->disk_name, "my_ramblcok");
 my_ramblock_disk->queue=my_ramblock_queue;
 set_capacity(my_ramblock_disk, RAMBLOCK_SIZE/512);
 /* 硬件相关操作 */
 my_ramblock_buf = kzalloc(RAMBLOCK_SIZE, GFP_KERNEL);
 
 add_disk(my_ramblock_disk);
 return 0;
 
 
}
 

static void my_ramblock_exit(void)
{
 unregister_blkdev(major, "my_ramblock");
 del_gendisk(my_ramblock_disk);
 put_disk(my_ramblock_disk);
 blk_cleanup_queue(my_ramblock_queue);
 kfree(my_ramblock_buf); 
}
 
module_init(my_ramblock_init);
module_exit(my_ramblock_exit);
MODULE_LICENSE("GPL");
