#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/slab.h>

#define BUFFER_SIZE 128
#define PROC_NAME "color"

struct color {
	int red;
	int blue;
	int green;
	struct list_head list;
};

static LIST_HEAD(color_list);

int proc_init(void)
{
	struct color *c;
	int rs[4] = {138, 256, 0, 0};
	int bs[4] = {42, 0, 0, 256};
	int gs[4] = {226, 0, 256, 0};
	int i;
	
	// initialize the list
	for(i = 0; i < 4; i++)
	{
		c = kmalloc(sizeof(*c), GFP_KERNEL);
		c->red = rs[i];
		c->blue = bs[i];
		c->green = gs[i];
		INIT_LIST_HEAD(&c->list);
	
		list_add_tail(&c->list, &color_list);
		
		printk(KERN_INFO "Added: R: %3d G: %3d B: %3d\n", c->red, c->green, c->blue);
	}
	
	// traverse the list
	list_for_each_entry(c, &color_list, list)
	{
		printk(KERN_INFO "R: %3d G: %3d B: %3d\n", c->red, c->green, c->blue);
	}
	
	return 0;
}

void proc_exit(void)
{
	struct color *c, *next;
	
	// delete the list
	list_for_each_entry_safe(c, next, &color_list, list)
	{
		printk(KERN_INFO "Removing: R: %3d G: %3d B: %3d\n", c->red, c->green, c->blue);
	
		list_del(&(c->list));
		kfree(c);
	}
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Module");
MODULE_AUTHOR("BZ");

