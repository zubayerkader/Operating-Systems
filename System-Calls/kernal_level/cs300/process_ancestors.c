#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <asm/current.h>
#include "process_ancestors.h"


asmlinkage long sys_process_ancestors(struct process_info info_array[], long size, long *num_filled)
{
	long filled = 0;
	struct process_info var;
	//struct process_info *ptrchk;
	int child_count = 0, sibling_count = 0, i = 1, j = 0;
	//struct list_head *head = current->children.next;
	struct list_head *traverser;// = head;
	struct task_struct *x = current;
	//struct cred *cred = current_cred();







	// checking size and pointers


	if (size <= 0)
		return -EINVAL;

	//if (copy_from_user(&filled, num_filled, sizeof(long)))
		//return -EFAULT;

	//if (copy_from_user(ptrchk, info_array, sizeof(var)))
		//return -EFAULT;





	// pid

	var.pid = current->pid;


	// name

	for (j = 0; j < 16; ++j)
	{
		var.name[j] = current->comm[j];
	}
	



	// state, uid, nvcsw, nivcsw

	var.state = current->state;







	

	//var.uid = cred->uid.val;
	//printk("[0] used current_cred(), uid: %li\n", var.uid);

	var.uid = current->cred->uid.val;
	//printk("[0] used x.cred->uid.val, uid: %li\n\n", var.uid);

	var.nvcsw = current->nvcsw;
	var.nivcsw = current->nivcsw;





	// children

	/*if (head == NULL)
		child_count = 0;
	if (head->next == head)
		child_count = 1;
	if (traverser != head)
	{
		traverser = head->next;
		child_count = 1;
		while (traverser != head)
		{
			child_count++;
			traverser = traverser->next;
		}
	}*/

	list_for_each(traverser, &current->children) 
	{
			
		child_count++;
			
	}

	var.num_children = child_count;




	// sibling

	/*head = current->sibling.next;
	traverser = head;
	if (head == NULL)
		sibling_count = 0;
	if (head->next == head)
		sibling_count = 1;
	if (traverser != head)
	{
		traverser = head->next;
		sibling_count = 1;
		while (traverser != head)
		{
			sibling_count++;
			traverser = traverser->next;
		}
	}*/

	list_for_each(traverser, &current->sibling) 
	{
			
		sibling_count++;
			
	}

	var.num_siblings = sibling_count;



	filled++;




	if (copy_to_user(&info_array[0], &var, sizeof(var)))
		return -EFAULT;

	

	
	if (current->parent == current)
	{
		if (copy_to_user(num_filled, &filled, sizeof(long)))
			return -EFAULT;
		return 0;
	}

	x = current->parent;
	while (x->parent != x && filled <= size-1)
	{
		//printk("ancestor[%d]\n", i);

		child_count = 0, sibling_count = 0;
		//head = x->children.next;
		//traverser = head;
		//if (copy_from_user(&var, &info_array[i], sizeof(var)))
			//return -EFAULT;

		var.pid = x->pid;
		for (j = 0; j < 16; ++j)
		{
			var.name[j] = x->comm[j];
		}
		var.state = x->state;



		//cred = current_cred();
		//var.uid = cred->uid.val;
		//printk("[%d] used current_cred(), uid: %li\n\n", i, var.uid);


		var.uid = x->cred->uid.val;
		//printk("[%d] used x.cred->uid.val, uid: %li\n\n", i, var.uid);




		var.nvcsw = x->nvcsw;
		var.nivcsw = x->nivcsw;

		/*if (head == NULL)
			child_count = 0;
		if (head->next == head)
			child_count = 1;
		if (traverser != head)
		{
			traverser = head->next;
			child_count = 1;
			while (traverser != head)
			{
				child_count++;
				traverser = traverser->next;
			}
		}*/

		list_for_each(traverser, &x->children) 
		{
			
			child_count++;
			
		}
		var.num_children = child_count;

		/*head = x->sibling.next;
		traverser = head;
		if (head == NULL)
			sibling_count = 0;
		if (head->next == head)
			sibling_count = 1;
		if (traverser != head)
		{
			traverser = head->next;
			sibling_count = 1;
			while (traverser != head)
			{
			
				sibling_count++;
				traverser = traverser->next;
			}
		}*/

		list_for_each(traverser, &x->sibling) 
		{
			sibling_count++;
		}
		var.num_siblings = sibling_count;


		if (copy_to_user(&info_array[i], &var, sizeof(var)))
			return -EFAULT;

		filled++;
		i++;
		x = x->parent;


	}



	if (x->parent == x && filled <= size-1)
	{
		printk("ancestor[%d]\n", i);

		child_count = 0, sibling_count = 0;
		//head = x->children.next;
		//traverser = head;
		//if (copy_from_user(&var, &info_array[i], sizeof(var)))
			//return -EFAULT;

		var.pid = x->pid;
		for (j = 0; j < 16; ++j)
		{
			var.name[j] = x->comm[j];
		}
		var.state = x->state;



		//cred = current_cred();
		//var.uid = cred->uid.val;
		//printk("[%d] used current_cred(), uid: %li\n\n", i, var.uid);


		var.uid = x->cred->uid.val;
		//printk("[%d] used x.cred->uid.val, uid: %li\n\n", i, var.uid);




		var.nvcsw = x->nvcsw;
		var.nivcsw = x->nivcsw;

		/*if (head == NULL)
			child_count = 0;
		if (head->next == head)
			child_count = 1;
		if (traverser != head)
		{
			traverser = head->next;
			child_count = 1;
			while (traverser != head)
			{
				child_count++;
				traverser = traverser->next;
			}
		}*/

		list_for_each(traverser, &x->children) 
		{
			
			child_count++;
			
		}
		var.num_children = child_count;

		/*head = x->sibling.next;
		traverser = head;
		if (head == NULL)
			sibling_count = 0;
		if (head->next == head)
			sibling_count = 1;
		if (traverser != head)
		{
			traverser = head->next;
			sibling_count = 1;
			while (traverser != head)
			{
			
				sibling_count++;
				traverser = traverser->next;
			}
		}*/

		list_for_each(traverser, &x->sibling) 
		{
			sibling_count++;
		}
		var.num_siblings = sibling_count;


		if (copy_to_user(&info_array[i], &var, sizeof(var)))
			return -EFAULT;

		filled++;
		//i++;
		//x = x->parent;


	}

	if (copy_to_user(num_filled, &filled, sizeof(long)))
			return -EFAULT;

	return 0;

}


	// long pid;                     /* Process ID */
 //    char name[ANCESTOR_NAME_LEN]; /* Program name of process */
 //    long state;                   /* Current process state */
 //    long uid;                     /* User ID of process owner */
 //    long nvcsw;                   /* # voluntary context switches */
 //    long nivcsw;                  /* # involuntary context switches */
 //    long num_children;            /* # children process has */
 //    long num_siblings; 			  /* # sibling process has */


