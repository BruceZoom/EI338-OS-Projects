#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

#define BUFFER_SIZE 128
#define PROC_NAME "ps_linear"

int proc_init(void)
{
	struct task_struct *task;
	for_each_process(task){
		printk(KERN_INFO "command = [%-15s] pid = [%5d] state = [%4ld]\n",
					task->comm, task->pid, task->state);
	}
	
	return 0;
}

void proc_exit(void)
{
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PS Linear Module");
MODULE_AUTHOR("BZ");

