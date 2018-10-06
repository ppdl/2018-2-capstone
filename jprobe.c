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

#define DEST_PORT		54321
#define SOURCE_PORT		54345
struct iothdr netstat;

int FDEFINE(dev_queue_xmit, 
		struct sk_buff *skb)
{
	struct tcphdr 			*th;
	struct iphdr			*ih;
	struct iothdr *ntst;
	int 					pktsize;

	ih = ip_hdr(skb);
	if(ih->protocol == 6) {
		th = tcp_hdr(skb);
		if(ntohs(th->dest) == DEST_PORT) {
			//msleep(30);		msleep? mdelay?
//			ntst = (struct iothdr *)skb->cb;
//			ntst->param1 = netstat.param1;
//			ntst->param2 = netstat.param2;
//			ntst->param3 = netstat.param3;
//			ntst->param4 = netstat.param4;
//			ntst->auth_key = netstat.auth_key;
//
//			ntst = (struct iothdr *)skb->cb;
//			printk(KERN_INFO"control block: %u %u %u %u %u\n", 
//					ntst->param1, ntst->param2, ntst->param3, ntst->param4, ntst->auth_key);

			printk("------------------\n");
		}
	}
	jprobe_return();
	return 0;
}
DECLARE_JPROBE(dev_queue_xmit);

int FDEFINE(__tcp_transmit_skb,
		struct sock *sk,
		struct sk_buff *skb,
		int clone_it,
		gfp_t gfp_mask,
		u32 rev_nxt)
{
	jprobe_return();
	return 0;
}
DECLARE_JPROBE(__tcp_transmit_skb);

int FDEFINE(ip_rcv, 
			struct sk_buff *skb)
{
	struct tcphdr 			*th;
	struct iphdr			*ih;
	struct iothdr 			*nstat;
//	printk(KERN_INFO"ip_rcv\n");
	ih = ip_hdr(skb);
	if(ih->protocol == 6) {
		th = tcp_hdr(skb);
		if(ntohs(th->dest) == SOURCE_PORT) {
			printk(KERN_ALERT"ip_rcv\n");
			nstat = (struct iothdr *)skb->cb;
			printk(KERN_INFO"rcv: %u, %u, %u, %u, %u\n",
					nstat->param1, nstat->param2, nstat->param3, nstat->param4, nstat->auth_key);
			printk(KERN_INFO"---------------------\n");
		}
	}
	jprobe_return();
	return 0;
}
DECLARE_JPROBE(ip_rcv);

int __init init_jp(void)
{
	netstat.param1=1;
	netstat.param2=23;
	netstat.param3=33;
	netstat.param4=123;
	netstat.auth_key=IOTYR_IDENTIFIER;
	REGISTER_JPROBE(dev_queue_xmit);
	REGISTER_JPROBE(__tcp_transmit_skb);
	REGISTER_JPROBE(ip_rcv);
	printk(KERN_ALERT"jprobe module start\n");
	return 0;
}

void __exit exit_jp(void)
{
	UNREGISTER_JPROBE(dev_queue_xmit);
	UNREGISTER_JPROBE(__tcp_transmit_skb);
	UNREGISTER_JPROBE(ip_rcv);
	printk(KERN_ALERT"jprobe module removed\n");
}

module_init(init_jp);
module_exit(exit_jp);

MODULE_AUTHOR("team IoTyranno");
MODULE_DESCRIPTION("simple jprobe test");
MODULE_LICENSE("GPL");
