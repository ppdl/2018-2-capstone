#include <linux/sched.h>
#include <asm/siginfo.h>
#include <linux/rcupdate.h>
#include <linux/signal.h>

struct task_struct* current_task;

void send_signal(int target_pid, int sig_num) 
{
	struct siginfo info;
	struct task_struct *t;

	memset(&info, 0, sizeof(struct siginfo));
	info.si_signo = sig_num;
	info.si_code = SI_QUEUE;
	info.si_int = 1234;

	rcu_read_lock();
	t = pid_task(find_vpid(target_pid), PIDTYPE_PID);
	if(t != NULL) {
		rcu_read_unlock();
		if(send_sig_info(sig_num, &info, t) < 0) printk("send_sig_info error\n");
	}
	else {
		printk("pid_task error\n");
		rcu_read_unlock();
	}
}
