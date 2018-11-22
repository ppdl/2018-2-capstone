#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>

#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/string.h>

struct file *file_open(const char *path, int flags, int rights) 
{
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    oldfs = get_fs();
    set_fs(get_ds());
    filp = filp_open(path, flags, rights);
    set_fs(oldfs);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

void file_close(struct file *file) 
{
    filp_close(file, NULL);
}

int file_read(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_read(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}   

int file_write(struct file *file, unsigned long long offset, unsigned char *data, unsigned int size) 
{
    mm_segment_t oldfs;
    int ret;

    oldfs = get_fs();
    set_fs(get_ds());

    ret = vfs_write(file, data, size, &offset);

    set_fs(oldfs);
    return ret;
}

//struct file* file;
int file_sync(struct file *file) 
{
    vfs_fsync(file, 0);
    return 0;
}

char buffer[100];
long tol;

long file_read_int(struct file* file)
{
	long ret;
	char buffer[100];

	memset(buffer, 0, 100);
	file_read(file, 0, buffer, 100);
	kstrtol(buffer, 10, &ret);
	return ret;
}
/*
int __init Init(void)
{
	file = file_open("target.txt", O_RDONLY, 0644);
	printk(KERN_INFO"buffer:%ld\n", file_read_int(file)); 
	return 0;
}

void __exit Exit(void)
{
	file_close(file);
}

module_init(Init);
module_exit(Exit);
*/
MODULE_LICENSE("GPL");

