#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kprobes.h>
#include <linux/delay.h>

int iotyr_handle_delay(struct kprobe *p, struct pt_regs *regs)
{
	printk(KERN_ALERT "tcp_transmit_skb, make delay here\n");
	//mdelay(300);
	return 0;
}

static struct kprobe kp = {
	.pre_handler = iotyr_handle_delay,
	.symbol_name = "__tcp_transmit_skb",
};

int __init init_delay_module(void)
{
	int ret;
	ret = register_kprobe(&kp);
	if (ret < 0)
	{
		printk(KERN_INFO"register_kprobe failed, return %d\n", ret);
		return ret;
	}
	printk(KERN_INFO"kprobe started\n");
	return 0;
}

void __exit exit_delay_module(void)
{
	unregister_kprobe(&kp);
	printk(KERN_INFO"kprobe removed\n");
}

module_init(init_delay_module);
module_exit(exit_delay_module);
MODULE_AUTHOR("team IoTyranno");
MODULE_DESCRIPTION("iotyr delay module");
MODULE_LICENSE("GPL");
