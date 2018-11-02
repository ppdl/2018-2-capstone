#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <linux/jiffies.h>

#define R_BITRATE	8

#define TERMINAL0	0xa02a00df	//10.42.0.223
#define TERMINAL1	0xa02a0002
#define TERMINAL2	0xa02a0003


struct TBI {
	unsigned long t;
	unsigned long b;
	unsigned long i;
	unsigned long c;
};

extern struct TBI tbi[3];
extern unsigned long last_jiffies;

extern const struct file_operations tbi_proc_fops;
void update_tbi_idle();
void update_tbi(int terminal_no, unsigned long transmit_time);
