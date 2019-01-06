#ifndef __KLOCK_H__
#define __KLOCK_H__

#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <semaphore.h>

typedef struct {
	 pthread_mutex_t mutex;
	 int lid;
} SmartLock;

void init_lock(SmartLock* lock);
int lock(SmartLock* lock);
void unlock(SmartLock* lock);
void cleanup();

#endif
