#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include "array_stats.h"

asmlinkage long sys_array_stats (struct array_stats *stats, long data[], long size)
{

	//might need malloc
	
	struct array_stats ptr;
	//struct array_stats ptr1;
	long var, min, max;
	long total = 0, i = 0;

	if (size <= 0)
		return -EINVAL;

	printk("passed size<=0 test\n");

	if (copy_from_user(&ptr, stats, sizeof(ptr)))
		return -EFAULT;

	printk("passed copy stats ptr test\n");

	if (copy_from_user(&min, &data[0], sizeof(long)))
			return -EFAULT;

	printk("passed copy data ptr test\n");

	max = min;
	for (i = 0; i < size; i++)
	{
		if (copy_from_user(&var, &data[i], sizeof(long)))
			return -EFAULT;
		total += var;
		if (var > max)
			max = var;
		if (var < min)
			min = var;
	}

	printk("populated min, max, total\n");

	ptr.min = min;
	ptr.max = max;
	ptr.sum = total;

	if (copy_to_user(stats, &ptr, sizeof(ptr)))
		return -EFAULT;

	printk("populated stat\n");

	return 0;
}
