#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include <linux/tcp.h>
#include <linux/ip.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include <linux/timer.h>

#include "tbi.h"

#define R_BITRATE	8

#define TERMINAL0	0xa2a00df	//10.42.0.223
#define TERMINAL1	0xa2a00d9	//10.42.0.217
#define TERMINAL2	0xa2a00be	//10.42.0.190

static int timer_interval = 500;
static void timer_handler(unsigned long data);

unsigned long last_jiffies;
unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct tcphdr *th;
	struct iphdr *ih;
	unsigned long transmit_time;
	
	th = 0;
	ih=ip_hdr(skb);
	if(ih->protocol == 6) th = tcp_hdr(skb);
	if(th) {
		transmit_time = msecs_to_jiffies(skb->data_len / R_BITRATE);
		printk(KERN_INFO"%x\n", ntohl(ih->saddr));
		if(ntohl(ih->saddr) == TERMINAL0) update_tbi(0, transmit_time);
		else if(ntohl(ih->saddr) == TERMINAL1) update_tbi(1, transmit_time);
		else if(ntohl(ih->saddr) == TERMINAL2) update_tbi(2, transmit_time);

		proc_create("tbi", 0, NULL, &write_tbi_info);		
	}
	return NF_ACCEPT;
}
static struct nf_hook_ops nfho = {
	.hook		= hook_func,
	.hooknum	= NF_INET_PRE_ROUTING,
	.pf			= PF_INET,
	.priority	= NF_IP_PRI_FIRST,
};


DEFINE_TIMER(timer_update_idle, (void*)timer_handler);
static void timer_handler(unsigned long data)
{
	update_tbi_idle();
	proc_create("tbi", 0, NULL, &write_tbi_info);
	mod_timer(&timer_update_idle, jiffies + msecs_to_jiffies(timer_interval));
}

int __init init_hello(void)
{
	last_jiffies = jiffies;
	memset(tbi, 0, sizeof(tbi));
	mod_timer(&timer_update_idle, jiffies + msecs_to_jiffies(timer_interval));
	nf_register_net_hook(&init_net, &nfho);
	return 0;
}

void __exit exit_hello(void)
{
	del_timer(&timer_update_idle);
	remove_proc_entry("tbi", NULL);
	nf_unregister_net_hook(&init_net, &nfho);
}

module_init(init_hello);
module_exit(exit_hello);

MODULE_AUTHOR("Team IoTyranno");
MODULE_DESCRIPTION("hub module");
MODULE_LICENSE("GPL");
