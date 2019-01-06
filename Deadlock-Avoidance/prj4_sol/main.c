#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "klock.h"
#include <pthread.h>

SmartLock glocks[2];

void *thread_0(void *arg) {
    while (lock(&glocks[0]) == 0); // Force locking glocks[0]
    sleep(1);
    while (lock(&glocks[1]) == 0); // Force locking glocks[1]

    printf("thread 0 is working on critical section for 1 second\n");
    sleep(1);

    unlock(&glocks[0]);
    unlock(&glocks[1]);

    return NULL;
}

void *thread_1(void *arg) {
    while (1) {
        int lock1_res = lock(&glocks[1]);
        sleep(2);
        if (lock1_res) {
            int lock0_res = lock(&glocks[0]);
            if (lock0_res) {
                printf("thread 1 is working on critical section for 1 second\n");
                sleep(1);
                unlock(&glocks[1]);
                unlock(&glocks[0]);
                break;
            } else {
                unlock(&glocks[1]);
                sleep(1);
            }
        }
    }
    return NULL;
}


int main(int argc, char *argv[]) {


    init_lock(&glocks[0]);
    init_lock(&glocks[1]);

    pthread_t tids[2];

    pthread_create(&tids[0], NULL, thread_0, NULL);
    pthread_create(&tids[1], NULL, thread_1, NULL);
    pthread_join(tids[0], NULL);
    pthread_join(tids[1], NULL);

    cleanup();
    return 0;
}
