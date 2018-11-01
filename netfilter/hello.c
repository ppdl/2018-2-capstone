#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>

#include <linux/tcp.h>
#include <linux/ip.h>

extern struct file* file_open(const char *path, int flags, int mode);
extern void file_close(struct file* file);
extern int file_write(struct file* file, unsigned long long* offset, const char* fmt, ...); 

unsigned long long foff;
struct file* fp;

unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	struct tcphdr *th;
	struct iphdr *ih;
	
	th = 0;
	ih=ip_hdr(skb);
	if(ih->protocol==6) th=tcp_hdr(skb);
	if(th) {
		file_write(fp, &foff, "s: %pI4, d: %pI4, seq: %u\n", &ih->saddr, &ih->daddr, ntohl(th->seq));
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
	fp = file_open("tbi_info.txt", O_CREAT|O_RDWR, 0644);
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
