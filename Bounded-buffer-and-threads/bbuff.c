#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <semaphore.h>
#include "bbuff.h"

void* buffer[BUFFER_SIZE];

sem_t mutex, full, empty;

int in=0, out=0;

void bbuff_init(void)
{
	sem_init (&mutex, 0, 1);
	sem_init (&full, 0, 0);
	sem_init (&empty, 0, BUFFER_SIZE);
}

void bbuff_blocking_insert(void* item)
{
	sem_wait (&empty);
	sem_wait (&mutex);
	
	buffer[in] = item;
	in = (in+1)%BUFFER_SIZE;

	sem_post (&mutex);
	sem_post (&full);
}

void* bbuff_blocking_extract(void)
{
	sem_wait (&full);
	sem_wait (&mutex);
	
	void* candy = buffer[out];
	buffer[out] = NULL;
	out = (out+1)%BUFFER_SIZE;

	sem_post (&mutex);
	sem_post (&empty);

	return candy;
}

_Bool bbuff_is_empty(void)
{
	int fullVal, emptyVal;
	sem_getvalue(&full, &fullVal);
	sem_getvalue(&empty, &emptyVal);

	if (fullVal == 0 && emptyVal == BUFFER_SIZE)
		return true;
	return false;
}