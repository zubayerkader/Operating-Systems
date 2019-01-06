#include <stdio.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include "process_ancestors.h"

#define _PROCESS_ANCESTOR 342


void print_process_info_array(struct process_info array[], long size)
{
	for (int i = 0; i < size; ++i)
	{
		printf("array[%d]:\n", i);
		printf("pid: %li\n", array[i].pid);
		printf("name: %s\n", array[i].name);
		printf("state: %li\n", array[i].state);
		printf("uid: %li\n", array[i].uid);
		printf("nvcsw: %li\n", array[i].nvcsw);
		printf("nivcsw: %li\n", array[i].nivcsw);
		printf("num_children: %li\n", array[i].num_children);
		printf("num_sibling: %li\n", array[i].num_siblings);
		printf("\n\n");
	}
}

int main()
{
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");
	

	int result = 1000;

	/*long num_filled = 0;
	long size = 0;
	struct process_info array_size_zero[0];

	printf("testing for size == 0\n");
	printf("\nDiving to kernel level\n\n");
	result = syscall(_PROCESS_ANCESTOR, array_size_zero, size, &num_filled);
	printf("\nRising to user level w/ result = %d\n\n", result);
	//assert(errno == EINVAL);
	if (result == -1)
	{
		printf("aborted because size is 0\n");
		exit(0);
	}
	printf("size != 0 passed\n");*/
	


	/*long *nullptrr = NULL;
	long size = 20;
	struct process_info *ptr = NULL;

	printf("testing for invalid pointers == 0\n");
	printf("\nDiving to kernel level\n\n");
	result = syscall(_PROCESS_ANCESTOR, ptr, size, nullptrr);
	printf("\nRising to user level w/ result = %d\n\n", result);
	//assert(errno == EFAULT);
	if (result == -1)
	{
		printf("aborted because of invalid pointer\n");
		exit(0);
	}
	printf("valid pointers\n");*/
	

	struct process_info array[20];
	long num_filled = 0;
	long size = 20;

	//fork_process();
	//fork_process();

	//pid_t arr[15];
	//int x = 0;

	for (int i = 1; i < 6; ++i)
	{
		//arr[i-1] = fork();
		pid_t child_pid = fork();
		if (child_pid > 0)
		{
			//arr[x] = child_pid;
			//x++ ;
			execlp ("/bin/ls", "ls -l", NULL);
		}
	}


	printf("\nDiving to kernel level\n\n");
	result = syscall(_PROCESS_ANCESTOR, array, size, &num_filled);
	printf("\nRising to user level w/ result = %d\n\n", result);

	print_process_info_array(array, num_filled);
	printf("num_filled: %li\n", num_filled);

	return 0;

}

