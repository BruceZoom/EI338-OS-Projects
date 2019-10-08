#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/moduleparam.h>

#define BUFFER_SIZE 128
#define PROC_NAME "collatz"

struct collatz {
	int value;
	struct list_head list;
};

static LIST_HEAD(collatz_list);

static int start = 25;

module_param(start, int, 0);

int proc_init(void)
{
	struct collatz *x;
	int v = start;
	
	// initialize the list
	x = kmalloc(sizeof(*x), GFP_KERNEL);
	x->value = v;
	INIT_LIST_HEAD(&x->list);
	list_add_tail(&x->list, &collatz_list);
	while(v != 1)
	{
		x = kmalloc(sizeof(*x), GFP_KERNEL);
		v = (v%2)?3*v+1:v/2;
		x->value = v;
		INIT_LIST_HEAD(&x->list);
	
		list_add_tail(&x->list, &collatz_list);
	}
	
	// traverse the list
	list_for_each_entry(x, &collatz_list, list)
	{
		printk(KERN_INFO "%d\n", x->value);
	}
	
	return 0;
}

void proc_exit(void)
{
	struct collatz *x, *next;
	
	// delete the list
	list_for_each_entry_safe(x, next, &collatz_list, list)
	{
		printk(KERN_INFO "Removing: %d\n", x->value);
	
		list_del(&(x->list));
		kfree(x);
	}
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Collatz Module");
MODULE_AUTHOR("BZ");

