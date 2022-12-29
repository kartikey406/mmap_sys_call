#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include<linux/sched.h>
#include<linux/mm.h>
#include <linux/mm_types.h>
#include<asm/page.h>
#include <linux/mm.h>
#include <asm/io.h>
#include <linux/slab.h>

#define FIRST_MINOR 0
#define MINOR_CNT 1

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;
static unsigned int *buffer;
static unsigned int array[10]={5000000,1,2,3,4,5,6,7,8,9};

static int my_open(struct inode *i, struct file *f)
{
	printk(KERN_INFO "IN OPEN FUNCTION \n");
    return 0;
}
static int my_close(struct inode *i, struct file *f)
{
    printk(KERN_INFO "IN CLOSE FUNCTION \n");
	return 0;
}

static ssize_t my_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "In read function \n");
	printk(KERN_ERR "VALUE OF BUFFER AT INDEX 2 IS %d \n",buffer[2]);
	return 0;
}
static ssize_t my_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_INFO "In write  function \n");
	return 0;
}


static int my_mmap(struct file *filp, struct vm_area_struct *vma)
{
    unsigned long page;
    unsigned char i;
    unsigned long start = (unsigned long)vma->vm_start;
    unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);

    page = virt_to_phys(buffer); // get the physical address from virtual.
    if(remap_pfn_range(vma, start, page>>PAGE_SHIFT, size, PAGE_SHARED))
        return -1;

    for(i=0; i<10; i++)  // write some data
        buffer[i] = array[i];

    return 0;
}

static struct file_operations driver_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write,
	.mmap=my_mmap
};


static int __init fcd_init(void)
{
	 
	int ret;
	struct device *dev_ret;

	if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "final_driver")) < 0)
	{
		return ret;
	}

	cdev_init(&c_dev, &driver_fops);

	if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
	{
		unregister_chrdev_region(dev, MINOR_CNT);
		return ret;
	}

	cl=class_create(THIS_MODULE,"mmap_testing");
	if (IS_ERR(cl))
	{
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(cl);
	}
	dev_ret=device_create(cl,NULL,MKDEV(MAJOR(dev),MINOR(dev)),NULL,"ab");
	if (IS_ERR(dev_ret))
	{
		class_destroy(cl);
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(dev_ret);
	}
	buffer = (int *)kmalloc(PAGE_SIZE, GFP_KERNEL);  // alloc some mem.
    //SetPageReserved(virt_to_page(buffer));  
	return 0;
}

static void __exit fcd_exit(void)
{
	device_destroy(cl,MKDEV(MAJOR(dev),MINOR(dev)));
	class_destroy(cl);
	cdev_del(&c_dev);
	unregister_chrdev_region(dev, MINOR_CNT);
}

module_init(fcd_init);
module_exit(fcd_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Deepanshu Kartikey <kartikey 406@gmail.com>");
MODULE_DESCRIPTION("MMAP testing char driver");
