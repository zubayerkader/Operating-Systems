#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include "stats.h"
#include "bbuff.h"

_Bool stop_thread = false;

typedef struct  
{
    int factory_number;
    double time_stamp_in_ms;
} candy_t;

double current_time_in_ms(void)
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

void* produce(void* arg)
{
	int facNum = *((int*)arg);
	srand(time(NULL));
	
	while (!stop_thread) 
	{
		int sec = rand()%4;
		printf("\tFactory %d ships candy & waits %ds\n", facNum, sec);
		candy_t *candy = malloc(sizeof(candy_t));
    	candy->factory_number = facNum;
    	candy->time_stamp_in_ms = current_time_in_ms();
    	stats_record_produced(facNum);
    	bbuff_blocking_insert(candy);
    	sleep(sec);
    }
    printf("Candy-factory %d done\n", facNum);
    pthread_exit(NULL);
}

void* consume(void* arg)
{
	srand(time(NULL));
	while(true)
	{
		candy_t *candy = bbuff_blocking_extract();	

		if (candy != NULL)
		{
			//stats
			//printf("this candy is produce by: %d\nin time: %f\n", candy->factory_number, (current_time_in_ms() - candy->time_stamp_in_ms));
			stats_record_consumed(candy->factory_number, (current_time_in_ms() - candy->time_stamp_in_ms));
			free(candy);
		}
		else
			printf("candy ptr is NULL is consumed function\n");

		int sec = rand()%2;
		sleep(sec);
	}
	pthread_exit(NULL);
}

int main(int argc, char* argv[])
{
	//1
	//extracting arguments
	if (argc!=4)
	{
		printf("error: invalid arguments\n");
		return 0;
	}

	int numfactories = atoi (argv[1]);
	int numkids = atoi (argv[2]);
	int numseconds = atoi (argv[3]);

	if (numfactories<=0 || numkids<=0 || numseconds<=0)
	{
		printf("error: invalid arguments\n");
		return 0;
	}

	//2
	//initialization
	bbuff_init();
	stats_init(numfactories);
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	//3
	//spawning factory threads
	int arr[numfactories];
	for (int i = 0; i < numfactories; i++)
		arr[i] = i;
	
	pthread_t factorythreads[numfactories];
	for (int i=0; i < numfactories; i++) 
	{
    	int ret = pthread_create(&factorythreads[i], &attr, produce, &arr[i]);

    	if(ret != 0) 
    	{
        	printf ("Error: pthread_create, factory_number: %d\n", arr[i]);
        	exit (1);
    	}
	}

	//4
	//spawning kid threads
	pthread_t kidthreads[numkids];
	for (int i=0; i < numkids; i++) 
	{
    	int ret = pthread_create(&kidthreads[i], &attr, consume, NULL);

    	if(ret != 0) 
    	{
        	printf ("Error: pthread_create, kid_number: %d\n", i);
        	exit (1);
    	}
	}

	//5
	//wait for request time
	int i = 0;
	printf("Time\t%ds:\n", i);
	for (i = 1; i < numseconds; i++)
	{
		sleep(1);
		printf("Time\t%ds:\n", i);
	}

	//6
	//stop factory threads
	stop_thread = true;
	printf("Stopping Facories Threads\n");
	for (int i = 0; i < numfactories; i++)
		pthread_join(factorythreads[i], NULL);

	//7
	//wait untill no more candy
	while (!bbuff_is_empty())
	{
		printf("Waiting for all candy to be consumed\n");
		sleep(1);
	}

	//8
	//stop kid threads
	printf("Stopping kid threads\n");
	for (int i = 0; i < numkids; i++)
	{
		pthread_cancel(kidthreads[i]);
		pthread_join(kidthreads[i], NULL);
	}

	//9
	//print stats
	stats_display(numfactories);

	//10
	//cleanup any allocated memory
	stats_cleanup();

	return 0;
}
