#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <semaphore.h>
#include "stats.h"

sem_t stat_mutex;

typedef struct  
{
    int made;
    int eaten;
    double min_delay;
    double avg_delay;
    double max_delay;
    double total_delay;
} record;

record *arr = NULL;

void compute_set_delays(int factory_number, double delay_in_ms)
{
	if (arr[factory_number].min_delay < 0)
		arr[factory_number].min_delay = delay_in_ms;

	if (arr[factory_number].min_delay > delay_in_ms)
		arr[factory_number].min_delay = delay_in_ms;

	if ( arr[factory_number].max_delay < delay_in_ms)
		arr[factory_number].max_delay = delay_in_ms;

	arr[factory_number].total_delay = arr[factory_number].total_delay + delay_in_ms;

	arr[factory_number].avg_delay = arr[factory_number].total_delay / arr[factory_number].eaten;
}

void stats_init(int num_producers)
{
	sem_init (&stat_mutex, 0, 1);
	arr = malloc(sizeof(record)*num_producers);
	for(int i=0; i< num_producers; i++)
	{
		arr[i].made = 0;
		arr[i].eaten = 0;
		arr[i].min_delay = -1;
		arr[i].avg_delay = 0;
		arr[i].max_delay= 0;
		arr[i].total_delay = 0;
	}
}

void stats_cleanup(void)
{
	free(arr);
	arr = NULL;
}

void stats_record_produced(int factory_number)
{
	sem_wait (&stat_mutex);
	
	arr[factory_number].made++;

	sem_post (&stat_mutex);
}

void stats_record_consumed(int factory_number, double delay_in_ms)
{
	sem_wait (&stat_mutex);
	
	compute_set_delays(factory_number, delay_in_ms);
	arr[factory_number].eaten++;

	sem_post (&stat_mutex);
}

void stats_display(int num_producers)
{
	printf ("Statistics:\n");
	printf ("%10s%8s%8s%15s%15s%15s\n", "Factory#", "#Made", "#Eaten", "Min Delay[ms]", "Avg Delay[ms]", "Max Delay[ms]");
	for (int i = 0; i<num_producers; i++)
	{		
		printf ("%10d%8d%8d%15.5f%15.5f%15.5f\n", i, arr[i].made, arr[i].eaten, arr[i].min_delay, arr[i].avg_delay, arr[i].max_delay);
		if (arr[i].eaten != arr[i].made)
			printf("ERROR:Mismatch between number made and eaten.\n");		
	}
}

