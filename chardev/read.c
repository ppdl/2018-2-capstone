#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>


#define MAX_BUFFER_SIZE 512
MODULE_LICENSE("GPL");

int result;
static char input_str[MAX_BUFFER_SIZE];
static unsigned long echo_buffer_size=0;

static ssize_t write_call_back_func(struct file *filep, const char *buffer, size_t len, loff_t *off)
{
	if(copy_from_user(input_str, buffer, echo_buffer_size)) return -EFAULT;
	printk(KERN_INFO "procfs_write: write %lu bytes, buffer: %s\n", echo_buffer_size, input_str);
	return echo_buffer_size;
}

int dummy(struct seq_file *m, void *v)
{
	return 0;
}

static int proc_open(struct inode* inode, struct file* file) 
{
	return single_open(file, dummy, NULL);
}

const struct file_operations proc_ops = {
	.owner = THIS_MODULE,
	.open = proc_open,
	.read = seq_read,
	.write = write_call_back_func,
	.llseek= seq_lseek,
	.release = single_release,
};

int init_module(void)
{
    printk("init printk example module\n");
	proc_create("rdrd", 0644, NULL, &proc_ops);

    return 0;
}

void cleanup_module(void)
{
    remove_proc_entry("rdrd", NULL);
    printk("exit printk example module\n");
}
