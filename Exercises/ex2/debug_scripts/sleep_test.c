#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "uthreads.h"

void sleeping_func(void) {
    printf("Thread starting, about to sleep\n");
    fflush(stdout);
    uthread_sleep(1);
    printf("Thread woke up\n");
}

void busy_loop_func(void) {
    printf("Busy thread\n");
    for (volatile int i = 0; i < 1000000; i++);
}

int main(void) {
    printf("Testing uthread_init...\n");
    if (uthread_init(10000) == -1) {  // 10ms quantum
        fprintf(stderr, "Init failed\n");
        return 1;
    }
    
    printf("Spawning sleeping thread...\n");
    int tid1 = uthread_spawn(sleeping_func);
    if (tid1 < 0) {
        fprintf(stderr, "Spawn failed\n");
        return 1;
    }
    
    printf("Spawning busy thread...\n");
    int tid2 = uthread_spawn(busy_loop_func);
    if (tid2 < 0) {
        fprintf(stderr, "Spawn failed\n");
        return 1;
    }
    
    printf("Threads spawned. Doing busy work...\n");
    
    // Do busy work to generate quantums
    for (volatile int i = 0; i < 10000000; i++);
    
    printf("Done\n");
    return 0;
}
