#include "collision.h"

unsigned int total_collision;

static int collision_proc_print(struct seq_file *m, void *v) {
	seq_printf(m, "%d", total_collision);
	return 0;
}
static int collision_proc_open(struct inode *inode, struct file *file) {
	return single_open(file, collision_proc_print, NULL);
}
const struct file_operations write_collision_info = {
	.owner = THIS_MODULE,
	.open = collision_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};
