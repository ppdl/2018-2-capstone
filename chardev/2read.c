#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>

static void read_file(char *filename)
{
	int fd;
	char buf[1];
	loff_t pos = 0;
	struct file *file;
	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);
	fd = sys_open(filename, O_RDONLY, 0);
	if(fd >= 0) 
	{
		file = fget(fd);
		if(file)
		{
			vfs_read(file, buf, sizeof(buf), &pos);
			printk(KERN_INFO"buf is %c\n", buf[0]);
		}
		sys_closd(fd);
	}
	set_fs(old_fs);
}

static int __init init(void) 
{
	printk(KERN_INFO"file handling start\n");
	read_file("target.txt");
	prkntk(KERN_INFO"file handling end\n");
	return 0;
}

static void __exit exit(void)

{
	printk(KERN_INFO"bye\n");
}

module_init(init);
module_exit(exit);<F3>
