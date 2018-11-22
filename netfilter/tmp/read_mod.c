#include <linux/module.h>  // Needed by all modules
#include <linux/kernel.h>  // Needed for KERN_INFO
#include <linux/fs.h>      // Needed by filp

#include <asm/segment.h>
#include <linux/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/syscalls.h>
#include <linux/fcntl.h>


struct file * f;

int __init init_mod(void)
{
	mm_segment_t oldfs;
	int ret;
	int fd;
	char buf[1];
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	fd = sys_open("target.txt", O_RDONLY, 0);
	if(fd >= 0) {
		while(sys_read(fd, buf, 1)==1) printk(KERN_INFO"%c", buf[0]);
		printk(KERN_INFO"\n");
		sys_close(fd);
	}
	set_fs(oldfs);

    return 0;
}

void __exit exit_mod(void)
{
    printk(KERN_INFO "My module is unloaded\n");
}

module_init(init_mod);
module_exit(exit_mod);
