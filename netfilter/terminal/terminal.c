#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include <linux/tcp.h>
#include <linux/ip.h>

#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#include "collision.h"

unsigned long last_sequence;

unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct tcphdr *th;
	struct iphdr *ih;
	unsigned long current_sequence;
	
	th = 0;
	ih=ip_hdr(skb);
	if(ih->protocol == 6) th = tcp_hdr(skb);
	if(th) {
		current_sequence = ntohl(th->seq);
		if(current_sequence <= last_sequence) total_collision++;
		else last_sequence = current_sequence;

		proc_create("collision_count", 0, NULL, &write_collision_info);		
	}
	return NF_ACCEPT;
}
static struct nf_hook_ops nfho = {
	.hook		= hook_func,
	.hooknum	= NF_INET_POST_ROUTING,
	.pf			= PF_INET,
	.priority	= NF_IP_PRI_FIRST,
};

int __init init_hello(void)
{
	last_sequence = 0;
	total_collision = 0;
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
MODULE_DESCRIPTION("terminal side module");
MODULE_LICENSE("GPL");
