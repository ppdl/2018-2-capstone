#include <linux/netfilter.h>
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

// flag: O_CREATE | O_APPEND
// mode: 0644
struct file *file_open(const char *path, int flags, int mode) 
{
	struct file* filp = NULL;
	mm_segment_t oldfs;
	int err = 0;

	oldfs = get_fs();
	set_fs(get_ds());
	filp = filp_open(path, flags, mode);
	set_fs(oldfs);
	if(IS_ERR(filp)) {
		err = PTR_ERR(filp);
		return NULL;
	}
	return filp;
}

void file_close(struct file *file)
{
	filp_close(file, NULL);
}

// offset: put a address of variable initialized by 0. automatically updated by vfs_write
int file_write(struct file *file, unsigned long long* offset, const char* fmt, ...)
{
	char buffer[256];
	mm_segment_t oldfs;
	int ret;

	va_list args;
	va_start(args, fmt);
	vsprintf(buffer, fmt, args);
	va_end(args);
	
	oldfs = get_fs();
	set_fs(get_ds());
	
	ret = vfs_write(file, buffer, strlen(buffer), offset);

	set_fs(oldfs);
	return ret;
}
