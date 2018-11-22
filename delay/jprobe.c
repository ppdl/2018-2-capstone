#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <linux/fs.h>
#include <net/sock.h>

#include "utility.h"

extern struct file* file_open(const char *path, int flags, int rights);
extern void file_close(struct file* file);
extern long file_read_int(struct file* file);

struct file* file;
int FDEFINE(__tcp_transmit_skb,
		struct sock *sk,
		struct sk_buff *skb,
		int clone_it,
		gfp_t gfp_mask,
		u32 rev_nxt)
{
	long delay;
	struct file* file;

	delay = file_read_int(file);
	udelay(delay);
	jprobe_return();
	return 0;
}
DECLARE_JPROBE(__tcp_transmit_skb);

int __init init_jp(void)
{
	file = file_open("../model/delay", O_RDONLY, 0644);
	REGISTER_JPROBE(__tcp_transmit_skb);
	printk(KERN_ALERT"jprobe module start\n");
	return 0;
}

void __exit exit_jp(void)
{
	file_close(file);
	UNREGISTER_JPROBE(__tcp_transmit_skb);
	printk(KERN_ALERT"jprobe module removed\n");
}

module_init(init_jp);
module_exit(exit_jp);

MODULE_AUTHOR("team IoTyranno");
MODULE_DESCRIPTION("simple jprobe test");
MODULE_LICENSE("GPL");
