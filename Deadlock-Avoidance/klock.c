#include "klock.h"

sem_t mut;

typedef struct Node{
	 long uid;
	 char type;
	 struct Node* ptr;
} Node;

Node* resource_list_ptr[1024];
int res_count = 0;
Node* thread_list_ptr[1024];
int thread_count = 0;

int lock_num = 0;

void init_lock(SmartLock* lock) 
{
	lock->lid = lock_num;
	pthread_mutex_init(&(lock->mutex), NULL);

	Node* head = malloc(sizeof(Node));
	head->uid = lock_num; 
	head->type = 'r';
	head->ptr = NULL;
	resource_list_ptr[res_count] = head;

	lock_num++;
	res_count++;
}

bool check_cycle(Node* start)
{
	Node* traverse_ptr = start;
	while (traverse_ptr->ptr != NULL)
	{
		traverse_ptr = traverse_ptr->ptr;
		if (traverse_ptr->uid == start->uid && traverse_ptr->type == start->type)
			return true;
	}
	return false;

}

int lock(SmartLock* lock) 
{
	sem_init (&mut, 0, 1);
	long x = pthread_self();
	Node* thread;

	bool exists = false;
	for (int i = 0; i < thread_count; i++)
		if (thread_list_ptr[i]->uid == x)
		{
			thread = thread_list_ptr[i];
			exists = true;
		}

	if (!exists)
	{
		thread = malloc(sizeof(Node));
		thread->uid = pthread_self(); 
		thread->type = 't';
		thread->ptr = NULL;
		sem_wait (&mut);
		thread_list_ptr[thread_count] = thread;
		thread_count++;
		sem_post (&mut);
	}

	Node* resource;
	for (int i = 0; i < res_count; i++)
	{
		if (resource_list_ptr[i]->uid == lock->lid)
		{
			resource = resource_list_ptr[i];
			break;
		}
	}

	thread->ptr = resource;

	bool cycle = check_cycle(thread);

	if (cycle)
	{
		thread->ptr = NULL;
		return 0;
	}
	else
	{
		if (resource->ptr == NULL)
		{
			pthread_mutex_lock(&(lock->mutex));
			printf("%lu locking\n", pthread_self());
			resource->ptr = thread;
			thread->ptr = NULL;
		}
		else
		{
			//while (resource->ptr != NULL);
			pthread_mutex_lock(&(lock->mutex));
			printf("%lu locking\n", pthread_self());
			resource->ptr = thread;
			thread->ptr = NULL;
		}
		return 1;
	}
}

void unlock(SmartLock* lock) 
{
	Node* resource;
	for (int i = 0; i < res_count; i++)
	{
		if (resource_list_ptr[i]->uid == lock->lid)
		{
			resource = resource_list_ptr[i];
			break;
		}
	}
	resource->ptr = NULL;
	pthread_mutex_unlock(&(lock->mutex)); //change line position
}

/*
 * Cleanup any dynamic allocated memory for SmartLock to avoid memory leak
 * You can assume that cleanup will always be the last function call
 * in main function of the test cases.
 */
void cleanup() 
{
	for (int i = 0; i < res_count; i++)
		free(resource_list_ptr[i]);

	for (int i = 0; i < thread_count; i++)
		free(thread_list_ptr[i]);
}












