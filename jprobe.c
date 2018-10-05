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
struct iotyr_net_status netstat;

int FDEFINE(dev_queue_xmit, 
		struct sk_buff *skb)
{
	struct tcphdr 			*th;
	struct iphdr			*ih;
	struct iotyr_net_status *ntst;
	int 					pktsize;

	ih = ip_hdr(skb);
	if(ih->protocol == 6) {
		th = tcp_hdr(skb);
		if(ntohs(th->dest) == DEST_PORT) {
			printk(KERN_INFO"headroom: %u, tailroom: %u\n", skb_headroom(skb), skb_tailroom(skb));
			printk(KERN_INFO"protocol:%d\n", ih->protocol); 
			printk(KERN_INFO"Source Address: %pI4 / Destination Address: %pI4\n", &ih->saddr, &ih->daddr);

			ntst = (struct iotyr_net_status*)skb->head;
			printk(KERN_INFO"%u, %u, %u, %u, %u\n",
					ntst->param1, ntst->param2, ntst->param3, ntst->param4, ntst->auth_key);
			pktsize = skb_headlen(skb);
			printk(KERN_INFO"packet size: %u\n", pktsize);
			printk(KERN_INFO"Is ACK packet? %d\n", th->ack);
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
	memcpy(skb->head, &netstat, sizeof(struct iotyr_net_status));
	jprobe_return();
	return 0;
}
DECLARE_JPROBE(__tcp_transmit_skb);

int FDEFINE(tcp_v4_rcv, 
			struct sk_buff *skb)
{
	struct tcphdr 			*th;
	struct iphdr			*ih;
	ih = ip_hdr(skb);
	if(ih->protocol == 6) {
		th = tcp_hdr(skb);
		if(ntohs(th->dest) == SOURCE_PORT) {
			printk(KERN_ALERT"tcp_v4_rcv\n");
			printk(KERN_INFO"---------------------\n");
		}
	}
	jprobe_return();
	return 0;
}
DECLARE_JPROBE(tcp_v4_rcv);

int __init init_jp(void)
{
	netstat.param1=1;
	netstat.param2=23;
	netstat.param3=33;
	netstat.param4=123;
	netstat.auth_key=IOTYR_IDENTIFIER;
	//REGISTER_JPROBE(dev_queue_xmit);
	//REGISTER_JPROBE(__tcp_transmit_skb);
	REGISTER_JPROBE(tcp_v4_rcv);
	printk(KERN_ALERT"jprobe module start\n");
	return 0;
}

void __exit exit_jp(void)
{
	//UNREGISTER_JPROBE(dev_queue_xmit);
	//UNREGISTER_JPROBE(__tcp_transmit_skb);
	UNREGISTER_JPROBE(tcp_v4_rcv);
	printk(KERN_ALERT"jprobe module removed\n");
}

module_init(init_jp);
module_exit(exit_jp);

MODULE_AUTHOR("team IoTyranno");
MODULE_DESCRIPTION("simple jprobe test");
MODULE_LICENSE("GPL");
