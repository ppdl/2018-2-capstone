#include <linux/proc_fs.h>
#include <linux/seq_file.h>

extern unsigned int total_collision;
extern const struct file_operations write_collision_info;
