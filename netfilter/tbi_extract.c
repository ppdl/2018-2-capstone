#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include <linux/tcp.h>
#include <linux/ip.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <linux/jiffies.h>

#include "tbi.h"

#define R_BITRATE	8

#define TERMINAL0	0xa02a00df	//10.42.0.223
#define TERMINAL1	0xa02a0002
#define TERMINAL2	0xa02a0003

extern struct file* file_open(const char *path, int flags, int mode);
extern void file_close(struct file* file);
extern int file_write(struct file* file, unsigned long long* offset, const char* fmt, ...); 

unsigned long long foff;
struct file* fp;

struct TBI tbi[3];
unsigned long last_jiffies;
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static struct proc_dir_entry *foo_proc_dir = NULL;
static struct proc_dir_entry *foo_proc_file = NULL;

static int hello_proc_show(struct seq_file *m, void *v) {
	seq_printf(m,"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu\n", 
				tbi[0].t, tbi[0].b, tbi[0].i,
				tbi[1].t, tbi[1].b, tbi[1].i,
				tbi[2].t, tbi[2].b, tbi[2].i);
  return 0;
}

static int hello_proc_open(struct inode *inode, struct  file *file) {
  return single_open(file, hello_proc_show, NULL);
}

static const struct file_operations hello_proc_fops = {
  .owner = THIS_MODULE,
  .open = hello_proc_open,
  .read = seq_read,
  .llseek = seq_lseek,
  .release = single_release,
};

unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct tcphdr *th;
	struct iphdr *ih;
	unsigned long idle_time;
	unsigned long transmit_time;
	
	idle_time = jiffies - last_jiffies;
	th = 0;
	ih=ip_hdr(skb);
	if(ih->protocol == 6) th = tcp_hdr(skb);
	if(th) {
		tbi[0].i += idle_time;
		tbi[1].i += idle_time;
		tbi[2].i += idle_time;
		transmit_time = msecs_to_jiffies(skb->data_len / R_BITRATE);
		if(ntohl(ih->saddr) == TERMINAL0) {
			tbi[0].t += transmit_time;
			tbi[1].b += transmit_time;
			tbi[2].b += transmit_time;
		}
		else if(ntohl(ih->saddr) == TERMINAL1) {
			tbi[0].b += transmit_time;
			tbi[1].t += transmit_time;
			tbi[2].b += transmit_time;
		}
		else if(ntohl(ih->saddr) == TERMINAL2) {
			tbi[0].b += transmit_time;
			tbi[1].b += transmit_time;
			tbi[2].t += transmit_time;
		}
		last_jiffies = jiffies;
//		file_write(fp,&foff,"%u,%u,%u,%u,%u,%u,%u,%u,%u\n", 
//				tbi[0].t, tbi[0].b, tbi[0].i,
//				tbi[1].t, tbi[1].b, tbi[1].i,
//				tbi[2].t, tbi[2].b, tbi[2].i);
		// give signal to user program
		foo_proc_dir = proc_mkdir("TBI", NULL);
		foo_proc_file = proc_create("tbi", 0, foo_proc_dir, &hello_proc_fops);
//		remove_proc_subtree("TBI", NULL);
//		remove_proc_entry("tbi", foo_proc_dir);
//		remove_proc_entry("TBI", NULL);
	}
	return NF_ACCEPT;
}

static struct nf_hook_ops nfho = {
	.hook		= hook_func,
	.hooknum	= NF_INET_PRE_ROUTING,
	.pf			= PF_INET,
	.priority	= NF_IP_PRI_FIRST,
};

int __init init_hello(void)
{
	foff = 0;
	last_jiffies = jiffies;
	fp = file_open("../log/tbi.txt", (O_CREAT|O_RDWR), 0644);
	/*	find delivery socket process id
	 *
	 * for_each_process(task) {
	 * if(!(strcmp(task->comm, "server.py")) process_id = task->pid;
	 *
	 */
	nf_register_net_hook(&init_net, &nfho);
	return 0;
}

void __exit exit_hello(void)
{
	file_close(fp);
	nf_unregister_net_hook(&init_net, &nfho);
}

module_init(init_hello);
module_exit(exit_hello);

MODULE_AUTHOR("Team IoTyranno");
MODULE_DESCRIPTION("collecting data");
MODULE_LICENSE("GPL");
