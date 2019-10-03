#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/hash.h>		// GOLDEN_RATIO_PRIME
#include <linux/gcd.h>		// gcd
#include <asm/param.h>		// HZ
#include <linux/jiffies.h>	// jiffies

long int clock;

int simple_init(void)
{
	printk(KERN_INFO "Loading Kernel Module\n");
	printk(KERN_INFO "%llu\n", GOLDEN_RATIO_PRIME);
	printk(KERN_INFO "%d\t%lu\n", HZ, jiffies);
	clock = jiffies;
	return 0;
}

void simple_exit(void)
{
	printk(KERN_INFO "Removing Kernle Module\n");
	printk(KERN_INFO "%lu\n", gcd(3300, 24));
	printk(KERN_INFO "%lu\n", jiffies);
	printk(KERN_INFO "%lu seconds have elapse before simple kernel is loaded\n", (jiffies - clock) / HZ);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("BZ");
