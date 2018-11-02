#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include <linux/tcp.h>
#include <linux/ip.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "tbi.h"

#define R_BITRATE	8

#define TERMINAL0	0xa02a00df	//10.42.0.223
#define TERMINAL1	0xa02a0002
#define TERMINAL2	0xa02a0003

unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct tcphdr *th;
	struct iphdr *ih;
	unsigned long transmit_time;
	
	idle_time = jiffies - last_jiffies;
	th = 0;
	ih=ip_hdr(skb);
	if(ih->protocol == 6) th = tcp_hdr(skb);
	if(th) {
		transmit_time = msecs_to_jiffies(skb->data_len / R_BITRATE);
		if(ntohl(ih->saddr) == TERMINAL0) update_tbi(0, transmit_time);
		else if(ntohl(ih->saddr) == TERMINAL1) update_tbi(1, transmit_time);
		else if(ntohl(ih->saddr) == TERMINAL2) update_tbi(2, transmit_time);

		proc_create("tbi", 0, NULL, &tbi_proc_fops);		
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
	last_jiffies = jiffies;
	memset(tbi, 0, sizeof(tbi));
	nf_register_net_hook(&init_net, &nfho);
	return 0;
}

void __exit exit_hello(void)
{
	remove_proc_entry("tbi", NULL);
	nf_unregister_net_hook(&init_net, &nfho);
}

module_init(init_hello);
module_exit(exit_hello);

MODULE_AUTHOR("Team IoTyranno");
MODULE_DESCRIPTION("collecting data");
MODULE_LICENSE("GPL");
