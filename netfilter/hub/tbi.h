#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <linux/jiffies.h>

#define R_BITRATE	8

struct TBI {
	unsigned long t;
	unsigned long b;
	unsigned long i;
	unsigned long c;
};

extern struct TBI tbi[3];
extern unsigned long last_jiffies;

extern const struct file_operations write_tbi_info;
void update_tbi_idle(void);
void update_tbi(int terminal_no, unsigned long transmit_time);
