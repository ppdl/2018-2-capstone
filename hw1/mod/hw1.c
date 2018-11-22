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
#include <asm/uaccess.h>

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
	extern variable for printing dead process.(For mode B)
	extern variable helps programmer to get the data from other source code.
*/
extern struct killedProcess *deadProcess;
extern struct killedProcess mylist;
extern struct list_head Node_head_ch;



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
	/*
		initialize.
		clear the list of kernel code.
		*/
	if(flag_check_mode == 'E'){
		list_for_each_safe(pos, q, &Node_head_ch){
		 	struct killedProcess *tmp;
			tmp = list_entry(pos, struct killedProcess, list);
			list_del(pos);
			kfree(tmp);
		}
	}
	return echo_buffer_size;
}

/*
	print scheduling policy!!.
	In Linux scheduler, there are three scheduling policies.
	First one is CFS.
	Second and thrid one is REAL-TIME(FIFO, RR).
	Last two policies are not user's scheduling policy.
	Lastly, there is a CFS, which is common scheduling policy for user process.
*/
static void print_sched(struct seq_file *s, struct task_struct *task){
	if(task->policy == 0) seq_printf(s, "%12s", "CFS");
	else if(task->policy == 1 || task->policy == 2) seq_printf(s, "%12s", "REALTIME");
	/*
		task->policy == 0 -> CFS (for normal task)
		task->policy == 1 -> FIFO (for real-time task)
		task->policy == 2 -> RR (for real-time task)
	*/
}

/*
	function for make bar.(Just printing bar in hw1's spec.)
*/
static void print_bar(struct seq_file *s){
	int i;
	for(i = 0;i < 160;i++)
		seq_printf(s, "-");
	seq_printf(s, "\n");
}

/*
	print error state when you don't echo E or T.
*/
int print_mode_error(struct seq_file *m, void *v){
	seq_printf(m, "Wrong input!! Enter 'E' or 'T'\n");
	return 0;
}

/*
	This is mode_B function.
	You should print dead process using kernel list.
*/
int print_mode_B(struct seq_file *m, void *v){
	struct killedProcess *p;
	struct list_head *pos, *q;
	unsigned long total_occupy_time_int, total_occupy_time_float;
	unsigned long start_time_i, start_time_f;
	
	print_bar(m);
	seq_printf(m, "%19s%8s%8s%8s%19s%19s\n", "command", "pid", "ppid", "cpu", "start(s)", "exit(s)");
	print_bar(m);
	/*
		When you use kernel list, then you must use macro named "list_for_each_entry".
		This macro should want iterator, head pointer, head_list data structure.
		Basically, kernel list is circular list. So, when you delete the data, then you should carefully write code
		for deleting something.(Deleting part is in proc_open function.)
	*/
	list_for_each_entry(p, &Node_head_ch, list){
	 	seq_printf(m, "%19s%8d%8d%8d", p->killed_command, p->pid, p->ppid, p->cpuNum);
		start_time_i = p->time_start_process / NSEC_PER_SEC;
		start_time_f = p->time_start_process / NSEC_PER_MSEC - (start_time_i * MSEC_PER_SEC);

		seq_printf(m, "%15lu.%03lu", start_time_i, start_time_f);
		total_occupy_time_float = jiffies_to_msecs(p->time_user_process + p->time_kernel_process) + (p->time_start_process / NSEC_PER_MSEC);
		total_occupy_time_int = total_occupy_time_float / MSEC_PER_SEC;
		total_occupy_time_float = total_occupy_time_float - (total_occupy_time_int * MSEC_PER_SEC);

		seq_printf(m, "%15lu.%03lu\n", total_occupy_time_int, total_occupy_time_float);
		
	}
	print_bar(m);
	/*
		initialize.
		clear the list of kernel code.
	*/
	list_for_each_safe(pos, q, &Node_head_ch){
	 	struct killedProcess *tmp;
		tmp = list_entry(pos, struct killedProcess, list);
		list_del(pos);
		kfree(tmp);
	}
	return 0;
}
/*
	2013147561 Song Chi Heon
	This is mode A of hw1.
	mode A prints several information from living process.
	Especially, mode A requires classify user process and kernel thread.
	Kernel threads shared their data and code from one process.
	So, they don't have mm(code, image, data)!!
*/
int print_mode_A(struct seq_file *m, void *v) {
	/*
		for printing all living task.
	*/
	struct task_struct *task = NULL;
	
	/*
		For classifying all the task with the task state condition.
	*/
	int total_num_of_process = 0;
	int num_of_sleep_process = 0;
	int num_of_running_process = 0;
	int num_of_stopped_process = 0;
	int num_of_zombie_process = 0;

	/*
		This is time after starting OS, booting time.
		All the time is u64. So, data-type is "unsigned long".
	*/
	unsigned long long boot_from_start_time;
	struct timespec current_time;
	unsigned long time_create_process_int, time_create_process_float;
	unsigned long time_user_occupy_float, time_user_occupy_int;
	unsigned long time_kernel_occupy_float, time_kernel_occupy_int;
	unsigned long total_occupy_time_float, total_occupy_time_int;

	/*
		This is variable for getting voluntary context-switching or in voluntary context-switching
	*/
	unsigned long voluntary_switch_count;
	unsigned long involuntary_switch_count;
	
	for_each_process(task){
		total_num_of_process = total_num_of_process + 1;
		switch(task->state){
			case 0:
				num_of_running_process = num_of_running_process + 1;
				break;
			case 1:
				num_of_sleep_process = num_of_sleep_process + 1;
				break;
			case 4:
				num_of_stopped_process = num_of_stopped_process + 1;
				break;
			case 32:
				num_of_zombie_process = num_of_zombie_process + 1;
				break;
			case 128:
				num_of_stopped_process = num_of_stopped_process + 1;
				break;
		}
	}

	print_bar(m);
	seq_printf(m, "%76s %-78s\n", "2013147561", "Song Chi Heon");
	print_bar(m);
	seq_printf(m, "CURRENT SYSTEM INFORMATION >\n");
	seq_printf(m, "Total %d task, %d running, %d sleeping, %d stopped, %d zombies\n",
		total_num_of_process, num_of_running_process, 
		num_of_sleep_process, num_of_stopped_process, 
		num_of_zombie_process);
	/*
		You should use get_monotonic_boottime
		This is function for return time_spec interface from ktime based data.
	*/
	get_monotonic_boottime(&current_time);

	boot_from_start_time = current_time.tv_sec * MSEC_PER_SEC + current_time.tv_nsec / NSEC_PER_MSEC ;
	seq_printf(m, "%dHz, %llu ms after system boot time\n", HZ, boot_from_start_time);
	print_bar(m);
	
	seq_printf(m, "%19s%8s%8s%13s%13s%13s%13s%14s%16s%8s%12s%15s\n",
		"command","pid","ppid","start(s)","total(s)",
		"user(s)","kernel(s)","voluntary","involuntary", "state",
		"scheduler", "vruntime");
	print_bar(m);
	/*
		This is process.
		Because, they have mm(code, image, data)
	*/
	for_each_process(task){
		if(task->mm != NULL){
			if(task->state == 64) continue;
			/*
				Create time is nsec unit. So you just calculate the time with sec.
			*/
			time_create_process_int = task->start_time / NSEC_PER_SEC;
			time_create_process_float = (task->start_time / NSEC_PER_MSEC) - (time_create_process_int * MSEC_PER_SEC);
			/*
				But user-time and kernel-time are a little bit different.
				They are calculated by the unit "jiffy".
				So you should convert them into msecs by using jiffes_to_msecs.
			*/
			time_user_occupy_float = jiffies_to_msecs(task->utime);
			time_user_occupy_int = time_user_occupy_float / MSEC_PER_SEC;
			time_user_occupy_float = time_user_occupy_float - (time_user_occupy_int * MSEC_PER_SEC);
			time_kernel_occupy_float = jiffies_to_msecs(task->stime);
			time_kernel_occupy_int = time_kernel_occupy_float / MSEC_PER_SEC;
			time_kernel_occupy_float = time_kernel_occupy_float - (time_kernel_occupy_int * MSEC_PER_SEC);
			total_occupy_time_float = jiffies_to_msecs(task->stime + task->utime);
			total_occupy_time_int = total_occupy_time_float / MSEC_PER_SEC;
			total_occupy_time_float = total_occupy_time_float - (total_occupy_time_int * MSEC_PER_SEC);
			/*
				These are variable for voluntary context-switch count and in-voluntary context-switch count.
				In the case of voluntary context-switch, a process  
			*/
			voluntary_switch_count = task->nvcsw;
			involuntary_switch_count = task->nivcsw;
			seq_printf(m, "%19s%8d%8d%9lu.%03lu%9lu.%03lu%9lu.%03lu%9lu.%03lu%14lu%16lu",
				task->comm, task->pid, task->parent->pid, 
				time_create_process_int, time_create_process_float, 
				total_occupy_time_int, total_occupy_time_float, 
				time_user_occupy_int, time_user_occupy_float, 
				time_kernel_occupy_int, time_kernel_occupy_float,
				voluntary_switch_count, involuntary_switch_count);

			switch(task->state){
				case 0:/*RUNNING*/
					/*
						vruntime is printed only when the task->state = TASK_RUNNING.
						Because, virtual runtime is calculated when they are switched from "expire" to "active".
						Higher priority is coming from lower vruntime.(in CFS, normal task)
					*/
					seq_printf(m, "%8c",'R');
					print_sched(m, task);
					seq_printf(m, "%15llu\n", task->se.vruntime);
					break;
				case 1:/*SLEEPING*/
					seq_printf(m, "%8c",'S');
					print_sched(m, task);
					seq_printf(m, "\n");
					break;
				case 4:/*STOPPED*/
					seq_printf(m, "%8c",'T');
					print_sched(m, task);
					seq_printf(m, "\n");
					break;
				case 32:/*ZOMBIE*/
					seq_printf(m, "%8c",'Z');
					print_sched(m, task);
					seq_printf(m, "\n");
					break;
				case 128:/*WAKEKILL : This is Also STOPPED*/
					seq_printf(m, "%8c",'T');
					print_sched(m, task);
					seq_printf(m, "\n");
					break;
			}
		}
	}
	print_bar(m);


	for_each_process(task){
		/*
			Threads share some code and data with one process.
			So, threads doesn't have mm structure.
			mm structure is code, image, data
		*/
		if(task->mm == NULL){
			if(task->state == 64) continue;
			/*
				Create time is nsec unit. So you just calculate the time with sec.
			*/
			time_create_process_int = task->start_time / NSEC_PER_SEC;
			time_create_process_float = (task->start_time / NSEC_PER_MSEC) - (time_create_process_int * MSEC_PER_SEC);

			/*
				But user-time and kernel-time are a little bit different.
				They are calculated by the unit "jiffy".
				So you should convert them into msecs by using jiffes_to_msecs.
			*/
			time_user_occupy_float = jiffies_to_msecs(task->utime);
			time_user_occupy_int = time_user_occupy_float / MSEC_PER_SEC;
			time_user_occupy_float = time_user_occupy_float - (time_user_occupy_int * MSEC_PER_SEC);
			time_kernel_occupy_float = jiffies_to_msecs(task->stime);
			time_kernel_occupy_int = time_kernel_occupy_float / MSEC_PER_SEC;
			time_kernel_occupy_float = time_kernel_occupy_float - (time_kernel_occupy_int * MSEC_PER_SEC);
			total_occupy_time_float = jiffies_to_msecs(task->stime + task->utime);
			total_occupy_time_int = total_occupy_time_float / MSEC_PER_SEC;
			total_occupy_time_float = total_occupy_time_float - (total_occupy_time_int * MSEC_PER_SEC);
			/*
				These are variable for voluntary context-switch count and in-voluntary context-switch count.
				In the case of voluntary context-switch, a process  
			*/
			voluntary_switch_count = task->nvcsw;
			involuntary_switch_count = task->nivcsw;
			seq_printf(m, "%19s%8d%8d%9lu.%03lu%9lu.%03lu%9lu.%03lu%9lu.%03lu%14lu%16lu",
				task->comm, task->pid, task->parent->pid, 
				time_create_process_int, time_create_process_float, 
				total_occupy_time_int, total_occupy_time_float, 
				time_user_occupy_int, time_user_occupy_float, 
				time_kernel_occupy_int, time_kernel_occupy_float,
				voluntary_switch_count, involuntary_switch_count);
			switch(task->state) {
				case 0:/*RUNNING*/
					/*
						vruntime is printed only when the task->state = TASK_RUNNING.
						Because, virtual runtime is calculated when they are switched from "expire" to "active".
						Higher priority is coming from lower vruntime.(in CFS, normal task)
					*/
					seq_printf(m, "%8c",'R');
					print_sched(m, task);
					seq_printf(m, "%15llu\n", task->se.vruntime);
					break;
					seq_printf(m, "\n");
				case 1:/*SLEEPING*/
					seq_printf(m, "%8c",'S');
					print_sched(m, task);
					seq_printf(m, "\n");
					break;
				case 4:/*STOPPED*/
					seq_printf(m, "%8c",'T');
					print_sched(m, task);
					seq_printf(m, "\n");
					break;
				case 32:/*ZOMBIE process*/
					seq_printf(m, "%8c", 'Z');
					print_sched(m, task);
					seq_printf(m, "\n");
					break;
				case 128:/*WAKE KILL process : This is Also STOPPED*/
					seq_printf(m, "%8c",'T');
					print_sched(m, task);
					seq_printf(m, "\n");
					break;
			}
		}
	}
	print_bar(m);
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
	if(flag_check_mode == 'T') return single_open(file, print_mode_A, NULL);
	else if(flag_check_mode == 'E') {
		return single_open(file, print_mode_B, NULL);
	}
	else return single_open(file, print_mode_error, NULL);
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
