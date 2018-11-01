#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <net/sock.h>

#include "utility.h"

int seqNum;
int FDEFINE(ip_finish_output2,
	struct net *net,
	struct sock *sk,
	struct sk_buff *skb)
{
	struct tcphdr *th;
	struct iphdr *ih;

	ih = ip_hdr(skb);
	if(ih->protocol == 6) {
		th = tcp_hdr(skb);
		printk(KERN_INFO "TX: %2d: %10u / %10u (%c)\n", seqNum++, ntohl(th->seq), ntohl(th->ack_seq), th->psh?'P':' ');
	}
	jprobe_return();
	return 0;
}
DECLARE_JPROBE(ip_finish_output2);

int FDEFINE(tcp_v4_rcv,
		struct sk_buff *skb) 
{
	struct tcphdr *th;
	th = tcp_hdr(skb);
	printk(KERN_INFO "RX: %2d: %10u / %10u (%c)\n", seqNum++, ntohl(th->seq), ntohl(th->ack_seq), th->psh?'P':' ');
	
	jprobe_return();
	return 0;
}
DECLARE_JPROBE(tcp_v4_rcv);

int __init init_jp(void)
{
	REGISTER_JPROBE(ip_finish_output2);
	REGISTER_JPROBE(tcp_v4_rcv);
	printk(KERN_ALERT"jprobe module start\n");
	return 0;
}

void __exit exit_jp(void)
{
	UNREGISTER_JPROBE(ip_finish_output2);
	UNREGISTER_JPROBE(tcp_v4_rcv);
	printk(KERN_ALERT"jprobe module removed\n");
}

module_init(init_jp);
module_exit(exit_jp);

MODULE_AUTHOR("team IoTyranno");
MODULE_DESCRIPTION("simple jprobe test");
MODULE_LICENSE("GPL");
