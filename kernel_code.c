#include <linux/init.h>
#include <linux/module.h>
#include<linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include<linux/slab.h>
#include <asm/uaccess.h>
#include <linux/module.h>

static int N=10;
module_param(N,int,S_IRUGO);		//Buffer Size

#define BUFFER_SIZE N
#define LINE_MAX 256		//Max Msg length
char **msg;
int readcount,writecount,opencount;
static DEFINE_SEMAPHORE(empty);		//Defination of semaphore
static DEFINE_SEMAPHORE(full);
static DEFINE_MUTEX(lock);
MODULE_LICENSE("DUAL BSD/GPL");

int openfns(struct inode *inode,struct file *filep);
int closefns(struct inode *inode,struct file *filep);
ssize_t readfns(struct file *filep,char *buff,size_t count,loff_t *offp );
ssize_t writefns(struct file *filep,const char *buff,size_t count,loff_t *offp );
char **data;
int readCount,writeCount;
int counter;

//Keeps track of all the file operations in this character device
struct file_operations my_fops={
        .open= openfns,
        .read= readfns,
        .write=writefns,
        .release=closefns
};

//Missolonus device with its file operations, permitions and name
static struct miscdevice process_list={
	.minor = MISC_DYNAMIC_MINOR,
	.name = "linepipe",
	.fops = &my_fops,
	.mode=0666
};

// called when module is installed
int __init init_module()
{
	int i;
	if(misc_register(&process_list)<0)
		printk(KERN_ALERT "Unable to register the device\n");
	readcount=0;writecount=0;
	sema_init(&empty, BUFFER_SIZE);
	sema_init(&full, 0);
	mutex_init(&lock);
	msg=kmalloc(sizeof(char *)*BUFFER_SIZE,GFP_KERNEL);
	for(i=0;i<BUFFER_SIZE;i++)
		msg[i]=kmalloc(sizeof(char)*LINE_MAX,GFP_KERNEL);
	return 0;
}
// called when module is removed
void __exit cleanup_module()
{
	int i;
	for(i=0;i<BUFFER_SIZE;i++)
		kfree(msg[i]);	
	kfree(msg);
	misc_deregister(&process_list);
	return ;
}

//Called when Read call is invoked
ssize_t readfns(struct file *filep,char *buffer,size_t count,loff_t *offp)
{
	int line_length=0;
	if(!(down_interruptible(&full)))
		if(!(mutex_lock_interruptible(&lock)))
		{	//Critical Section Begin
			line_length=strlen(msg[readcount]);
			/* function to copy kernel space buffer to user space*/
			if (copy_to_user(buffer,msg[readcount],LINE_MAX) != 0 )
				printk( "Kernel -> userspace copy failed!\n" );
			readcount++;
			readcount=readcount%BUFFER_SIZE;
			//critical Section End
			mutex_unlock(&lock);
			up(&empty);

		}
	return line_length;
}

//Called when Write call is Invoked in User code
ssize_t writefns(struct file *filep,const char *buffer,size_t count,loff_t *offp)
{
	int line_length=0;
	if(!(down_interruptible(&empty)))
		if(!(mutex_lock_interruptible(&lock)))
		{//Critical Section Begin
			line_length=strlen(buffer);
			//function to copy data from user space to kernal space
			if (copy_from_user(msg[writecount],buffer,LINE_MAX) != 0 )
				printk( "Kernel -> userspace copy failed!\n" );
			writecount++;
			writecount=writecount%BUFFER_SIZE;
			//Critical Section End
			mutex_unlock(&lock);
			up(&full);
		}
return line_length;
}

//Caled then the character device is opened
int openfns(struct inode *inode,struct file *filep)
{
	opencount++;// keeps count of no of devices pened thsi file	

	return 0;
}

//Called when character device is closed
int closefns(struct inode *inode,struct file *filep)
{
	if (opencount>0)	//decreases the no of process opened the character device
		opencount--;
	return 0;
}
