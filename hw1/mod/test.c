/*
	Name : Song Chi Heon
	Student ID : 2013147561
	Major : Computer Science
*/
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/time.h>

/*
	extern variable's data structure is in linux/sched.h
*/
#include <linux/sched.h>

#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

/*
	You should include linux/list.h for using kernel list.
*/
#include <linux/list.h>

MODULE_LICENSE("GPL");
#define FILENAME "hw1"

/*
	Buffer size of echo.
*/
#define MAX_BUFFER_SIZE 512
static char input_str[MAX_BUFFER_SIZE];
static unsigned long echo_buffer_size = 0;
//static struct proc_dir_entry *Our_Proc_File;


/*
	Global variables for deciding mode.
*/
char flag_check_mode = 'T';

/*
	This is call back function for write.
	When you do echo 'E' > /proc/hw1,
	then this write_call_back_func function is operated.
	It is just a code "from googling".
	Refernece Link : https://github.com/juicyjelly74/linux_lab5/blob/master/custom_calculator.c
*/
static ssize_t write_call_back_func(struct file *filp, const char *buffer, size_t len, loff_t *off)
{
	struct list_head *pos, *q;
	if(len > MAX_BUFFER_SIZE){
		echo_buffer_size = MAX_BUFFER_SIZE;
	}
	else{
		echo_buffer_size = len;
	}
	if(copy_from_user(input_str, buffer, echo_buffer_size)) return -EFAULT;
	printk(KERN_INFO "procfs_write: write %lu bytes, buffer: %s\n", echo_buffer_size, input_str);
	flag_check_mode = input_str[0];
	return echo_buffer_size;
}

int print_mode_A(struct seq_file *m, struct file* file)
{
	return 0;
}
/*
	open_selected_proc is for executing everytime when you do 'cat /proc/hw1'.
	We have global variable "flag_check_mode".
	If the flag_check_mode is 'T', then show mode A when cat /proc/hw1.
	If the flag_check_mode is 'E', then show mode B when cat /proc/hw1.
	In other case, print error message.
*/
int open_selected_proc(struct inode *inode, struct  file *file) {
	return single_open(file, print_mode_A, NULL);
}

/*
	This is list of call back function(you may hear this concept in Computer Graphics.)
*/
const struct file_operations proc_ops = {
	.owner = THIS_MODULE,
	.open = open_selected_proc,
	.read = seq_read,
	.write = write_call_back_func,
	.llseek = seq_lseek,
	.release = single_release,
};

/*
	make /proc/hw1 file
*/
int __init proc_init(void) {
	proc_create(FILENAME, 0, NULL, &proc_ops);
	return 0;
}

/* 
	when module unloaded, remove /proc/hw1 file
*/
void __exit proc_exit(void) {
	remove_proc_entry(FILENAME, NULL);
}

module_init(proc_init);
module_exit(proc_exit);
