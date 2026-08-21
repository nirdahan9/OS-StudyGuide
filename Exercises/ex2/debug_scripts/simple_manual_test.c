#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "uthreads.h"

void simple_func(void) {
    printf("Thread executed\n");
}

int main(void) {
    printf("Testing uthread_init...\n");
    if (uthread_init(10000) == -1) {
        fprintf(stderr, "Init failed\n");
        return 1;
    }
    
    printf("Spawning thread...\n");
    int tid = uthread_spawn(simple_func);
    if (tid < 0) {
        fprintf(stderr, "Spawn failed\n");
        return 1;
    }
    
    printf("Thread spawned with TID %d\n", tid);
    
    // Let thread run
    usleep(100000);
    
    printf("Done\n");
    return 0;
}
