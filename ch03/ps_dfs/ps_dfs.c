#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

#define BUFFER_SIZE 128
#define PROC_NAME "ps_dfs"

void dfs(struct task_struct *parent)
{
	struct list_head *list;
	struct task_struct *task;
	
	// end of DFS
	if(parent == NULL)
	{
		return;
	}
	
	// print task info
	printk(KERN_INFO "command = [%-15s] pid = [%5d] state = [%4ld]\n",
					parent->comm, parent->pid, parent->state);
	
	// next level
	list_for_each(list, &(parent->children))
	{
		task = list_entry(list, struct task_struct, sibling);
		dfs(task);
	}
}

int proc_init(void)
{
	dfs(&init_task);
	
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

