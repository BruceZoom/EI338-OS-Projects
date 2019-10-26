#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define BUFFER_SIZE 128
#define PROC_NAME "pid"

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos);

static struct file_operations proc_ops = {
	.owner = THIS_MODULE,
	.read = proc_read,
	.write = proc_write
};

long pid_num;

int proc_init(void)
{
	proc_create(PROC_NAME, 0666, NULL, &proc_ops);
	return 0;
}

void proc_exit(void)
{
	remove_proc_entry(PROC_NAME, NULL);
}

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
	int rv = 0;
	char buffer[BUFFER_SIZE];
	static int completed = 0;
	
	struct task_struct *task;
	
	if(completed){
		completed = 0;
		return 0;
	}
	completed = 1;
	
	// retrieve task info
	task = pid_task(find_vpid(pid_num), PIDTYPE_PID);
	if(task == NULL)
	{
		return 0;	// comment this line to print error message
		rv = sprintf(buffer, "process %ld not found\n", pid_num);
		copy_to_user(usr_buf, buffer, rv);
		return rv;
	}
	rv = sprintf(buffer, "command = [%s] pid = [%d] state = [%ld]\n",
					task->comm, task->pid, task->state);
	copy_to_user(usr_buf, buffer, rv);
	
	return rv;
}

ssize_t proc_write(struct file *file, const char __user *usr_buf, size_t count, loff_t *pos)
{
	int rv = 0;
	char *k_mem;
	
	k_mem = kmalloc(count+1, GFP_KERNEL);
	
	copy_from_user(k_mem, usr_buf, count);
	
	// '\0' must be appended to convert the string into long
	k_mem[count] = '\0';
	rv = kstrtol(k_mem, 10, &pid_num);
	
	kfree(k_mem);
	
	return count;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("PID Module");
MODULE_AUTHOR("BZ");

