#include "tbi.h"

struct TBI tbi[3];
unsigned long last_jiffies;

static int tbi_proc_print(struct seq_file *m, void *v) {
	seq_printf(m,"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu\n", 
				tbi[0].t, tbi[0].b, tbi[0].i,
				tbi[1].t, tbi[1].b, tbi[1].i,
				tbi[2].t, tbi[2].b, tbi[2].i);
	return 0;
}

static int tbi_proc_open(struct inode *inode, struct  file *file) {
	return single_open(file, tbi_proc_print, NULL);
}

const struct file_operations tbi_proc_fops = {
	.owner = THIS_MODULE,
	.open = tbi_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

void update_tbi_idle()
{
	unsigned long idle_time = jiffies - last_jiffies;

	tbi[0].i += idle_time;
	tbi[1].i += idle_time;
	tbi[2].i += idle_time;

	last_jiffies = jiffies;
}

void update_tbi(int terminal_no, unsigned long transmit_time)
{
	update_tbi_idle();

	if(terminal_no == 0) {
		tbi[0].t += transmit_time;
		tbi[1].b += transmit_time;
		tbi[2].b += transmit_time;
	}
	else if(terminal_no == 1) {
		tbi[0].b += transmit_time;
		tbi[1].t += transmit_time;
		tbi[2].b += transmit_time;
	}
	else if(terminal_no == 2) {
		tbi[0].b += transmit_time;
		tbi[1].b += transmit_time;
		tbi[2].t += transmit_time;
	}
	last_jiffies = jiffies;
}
