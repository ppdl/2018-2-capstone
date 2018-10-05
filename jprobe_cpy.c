#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/skbuff.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <net/sock.h>

#define IOTYR_IDENTIFIER	0X4ae7
struct iotyr_net_status {
	__u64	param1:8,
			param2:8,
			param3:8,
			param4:8,
			auth_key:16;
};

struct iotyr_net_status netstat;

int iotyr_delay(struct sk_buff *skb) 
{
	struct tcphdr 			*th;
	struct iphdr			*ih;
	struct iotyr_net_status ntst;
	struct iotyr_net_status tnst;
	int 					pktsize;

	ih = ip_hdr(skb);
	if(ih->protocol == 6) {
		th = tcp_hdr(skb);
		if(ntohs(th->dest) == 54321) {
			printk(KERN_INFO"headroom: %u, tailroom: %u\n", skb_headroom(skb), skb_tailroom(skb));
			printk(KERN_INFO"protocol:%d\n", ih->protocol); 
			printk(KERN_INFO"Source Address: %pI4 / Destination Address: %pI4\n", &ih->saddr, &ih->daddr);

			memcpy(&tnst, skb->head, sizeof(struct iotyr_net_status));
			printk(KERN_INFO"delay: %u, %u, %u, %u, %u\n", tnst.param1, tnst.param2, tnst.param3, tnst.param4, tnst.auth_key);
			
			pktsize = skb_headlen(skb);
			printk(KERN_INFO"packet size: %u\n", pktsize);
			printk(KERN_INFO"Is ACK packet? %d\n", th->ack);
			printk("------------------\n");
		}
	}
	jprobe_return();
	return 0;
}
int iotyr_trans_info(struct sock *sk, struct sk_buff *skb, int clone_it, gfp_t gfp_mask, u32 rev_nxt)
{
	struct tcphdr 			*th;
	struct iphdr			*ih;
	struct iotyr_net_status ntst;
	char*	iter;
	int 	pktsize;

	ih = ip_hdr(skb);
	if(ih->protocol == 6) {
		th = tcp_hdr(skb);
		if(ntohs(th->dest) == 54321) {
			memcpy(skb->head, &netstat, sizeof(struct iotyr_net_status));
			printk(KERN_INFO"make transmission info\n");
			iter = skb->head;
			printk(KERN_ALERT"info: %u, %u, %u, %u\n", *iter, *(iter+1), *(iter+2), *(iter+3));
		}
	}
	jprobe_return();
	return 0;
}
static struct jprobe iotyr_delay_probe = {
	.entry = iotyr_delay,
	.kp = {
		.symbol_name = "dev_queue_xmit",
	},
};
static struct jprobe iotyr_transmit_info = {
	.entry = iotyr_trans_info,
	.kp = {
		.symbol_name = "__tcp_transmit_skb",
	},
};

int __init init_jp(void)
{
	netstat.param1=1;
	netstat.param2=23;
	netstat.param3=33;
	netstat.param4=123;
	netstat.auth_key=IOTYR_IDENTIFIER;
	register_jprobe(&iotyr_delay_probe);
	register_jprobe(&iotyr_transmit_info);
	printk(KERN_ALERT"jprobe module start\n");
	return 0;
}

void __exit exit_jp(void)
{
	unregister_jprobe(&iotyr_delay_probe);
	unregister_jprobe(&iotyr_transmit_info);
	printk(KERN_ALERT"jprobe module removed\n");
}

module_init(init_jp);
module_exit(exit_jp);

MODULE_AUTHOR("team IoTyranno");
MODULE_DESCRIPTION("simple jprobe test");
MODULE_LICENSE("GPL");
