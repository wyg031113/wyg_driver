/*************************************************************************
    > File Name: blockdev.c
    > Author: wyg
    > Mail: wyg_0802@126.com 
    > Created Time: 2016年12月15日 星期四 16时16分06秒
 ************************************************************************/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/blkdev.h>
#include <linux/genhd.h>
#include <linux/fs.h>
#include <linux/bio.h>

#define debug(fmt, ...) printk(KERN_DEBUG fmt, ##__VA_ARGS__)
#define SECTOR_SIZE 512
#define N_SEC 1024
#define DEV_SIZE (SECTOR_SIZE * N_SEC)
#define BLK_MAJOR 0
static char blk_dev_name[] = "wyg_blk";
static char flash[DEV_SIZE];
static int major;
spinlock_t lock;
static struct gendisk *gd;
static struct request_queue *blkdev_queue;
static int simple_blkdev_make_request(struct request_queue *q, struct bio *bio)
{
	struct bio_vec *bvec;
	int i;
	void *dsk_mem;
	if((bio->bi_sector * SECTOR_SIZE) + bio->bi_size > DEV_SIZE){
		debug("bad request, too large.\n");
		bio_endio(bio, -EIO);
		return 0;
	}
	dsk_mem = flash + (bio->bi_sector << 9);
	bio_for_each_segment(bvec, bio, i){
		void *iovec_mem;
		switch(bio_rw(bio)){
			case READ:
			case READA:
				iovec_mem = kmap(bvec->bv_page) + bvec->bv_offset;
				memcpy(iovec_mem, dsk_mem, bvec->bv_len);
				kunmap(bvec->bv_page);
				break;
			case WRITE:
				iovec_mem = kmap(bvec->bv_page) + bvec->bv_offset;
				memcpy(dsk_mem, iovec_mem, bvec->bv_len);
				kunmap(bvec->bv_page);
				break;
			default:
				debug("unknow value of bio rw.\n");
				bio_endio(bio, -EIO);
				return 0;
		}
		dsk_mem += bvec->bv_len;
	}
	bio_endio(bio, 0);
	return 0;
}

static int blk_ioctl(struct block_device *dev, fmode_t no, unsigned cmd, unsigned long arg)
{
	return -ENOTTY;
}
static int blk_open(struct block_device *dev, fmode_t no)
{
	debug("block device opened.\n");
	return 0;
}
static int blk_release(struct gendisk *gd, fmode_t no)
{
	debug("block device released.\n");
	return 0;
}
struct block_device_operations blk_ops = 
{
	.owner		=	THIS_MODULE,
	.open		=	blk_open,
	.release	=	blk_release,
	.ioctl		=	blk_ioctl,
};
static int __init blk_init(void)
{
	int ret = 0;
	debug("blk init.\n");
	blkdev_queue = blk_alloc_queue(GFP_KERNEL);
	if(!blkdev_queue){
		ret = -ENOMEM;
		goto failed1;
	}
	blk_queue_make_request(blkdev_queue, simple_blkdev_make_request);

	if((major = register_blkdev(0, blk_dev_name)) < 0){
		debug("register blkdev failed.\n");
		ret = -EBUSY;
		goto failed2;
	}
	gd = alloc_disk(1);
	if(IS_ERR(gd)){
		debug("alloc disk failed.\n");
		ret = -ENOMEM;
		goto failed3;
	}
	spin_lock_init(&lock);
	gd->major = major;
	gd->first_minor = 0;
	gd->fops = &blk_ops;
	gd->queue = blkdev_queue;
	snprintf(gd->disk_name, 32, "blk%c", 'a');
	set_capacity(gd, N_SEC);
	add_disk(gd);
	debug("gendisk add successfully.\n");
	return 0;
failed3:
	unregister_blkdev(major, blk_dev_name);
failed2:
	blk_cleanup_queue(blkdev_queue);
failed1:
	return ret;
}

static void __exit blk_exit(void)
{
	del_gendisk(gd);
	blk_cleanup_queue(gd->queue);
	unregister_blkdev(major, blk_dev_name);
	debug("blk exited.\n");

}

module_init(blk_init);
module_exit(blk_exit);
MODULE_LICENSE("GPL");
