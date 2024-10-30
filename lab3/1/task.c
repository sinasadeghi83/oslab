#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>

/* this function is called when the module is loaded*/
int task_init(void)
{
    pr_info("Loading \"task\" module");
// is alias to -> printk(KERN_INFO "Loading Module\n");
    return 0;
}

void task_exit(void)
{
    pr_info("Removing \"task\" module\n");
}

module_init(task_init);
module_exit(task_exit);

MODULE_LICENSE("GPL");