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
bool syn_require;
bool syn_acked;
bool fin_require;
bool init_sequence;
bool connected;

#define PAYLOAD_SIZE(skb)	((skb)->tail - (skb)->head - (skb)->transport_header - tcp_hdr(skb)->doff*4)

unsigned int post_hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct tcphdr *th;
	struct iphdr *ih;
	unsigned long current_sequence;
	
	th = 0;
	ih=ip_hdr(skb);
	if(ih->protocol == 6) th = tcp_hdr(skb);
	if(th) {
		if(th->syn) {
			syn_require = true;
			init_sequence = false;
		}
		if(th->fin) fin_require = true;

		if(connected && PAYLOAD_SIZE(skb)) {
			//printk("size: %d\n", skb->tail - skb->head - skb->transport_header - th->doff*4); 
			current_sequence = ntohl(th->seq);
			if(!init_sequence) {
				init_sequence = true;
				last_sequence = current_sequence - 1;
				//printk(KERN_INFO"*init seq: %lu\n", last_sequence);
			}
			if(current_sequence <= last_sequence) {
				//printk(KERN_INFO"collision - last: %lu, curr: %lu\n", last_sequence, current_sequence);
				total_collision++;
			}
			else {
				//printk(KERN_INFO"pass - update last to: %lu\n", current_sequence);
				last_sequence = current_sequence;
			}
		}

		remove_proc_entry("collision_count", NULL);
		proc_create("collision_count", 0, NULL, &write_collision_info);	
		if(syn_acked) {
			syn_acked = false;
			syn_require = false;
			connected = true;
		}
	}
	return NF_ACCEPT;
}
static struct nf_hook_ops nfho1 = {
	.hook		= post_hook_func,
	.hooknum	= NF_INET_POST_ROUTING,
	.pf			= PF_INET,
	.priority	= NF_IP_PRI_FIRST,
};

unsigned int pre_hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct tcphdr *th;
	struct iphdr *ih;
	
	th = 0;
	ih = ip_hdr(skb);
	if(ih->protocol == 6) th = tcp_hdr(skb);
	if(th) {
		if(syn_require && th->syn) syn_acked = true;
		if(fin_require && th->fin) {
			fin_require = false;
			connected = false;
			init_sequence = false;
		}
	}
	return NF_ACCEPT;
}
static struct nf_hook_ops nfho2 = {
	.hook		= pre_hook_func,
	.hooknum	= NF_INET_PRE_ROUTING,
	.pf			= PF_INET,
	.priority	= NF_IP_PRI_FIRST,
};

int __init init_hello(void)
{
	last_sequence = 0;
	total_collision = 0;
	syn_require = false;
	syn_acked = false;
	fin_require = false;
	connected = false;
	nf_register_net_hook(&init_net, &nfho1);
	nf_register_net_hook(&init_net, &nfho2);
	return 0;
}

void __exit exit_hello(void)
{
	remove_proc_entry("collision_count", NULL);
	nf_unregister_net_hook(&init_net, &nfho1);
	nf_unregister_net_hook(&init_net, &nfho2);
}

module_init(init_hello);
module_exit(exit_hello);

MODULE_AUTHOR("Team IoTyranno");
MODULE_DESCRIPTION("terminal side module");
MODULE_LICENSE("GPL");
