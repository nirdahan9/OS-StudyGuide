#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "uthreads.h"

int signal_count = 0;

void signal_handler_test(int sig) {
    signal_count++;
    printf("Signal %d received (count=%d)\n", sig, signal_count);
}

void sleeping_func(void) {
    printf("Thread starting, about to sleep\n");
    fflush(stdout);
    uthread_sleep(1);
    printf("Thread woke up\n");
}

void busy_loop_func(void) {
    printf("Busy thread running\n");
    fflush(stdout);
    for (volatile int i = 0; i < 1000000; i++);
}

int main(void) {
    printf("Testing ITIMER signal...\n");
    
    // Install custom signal handler to see if signals are generated
    signal(SIGVTALRM, signal_handler_test);
    
    printf("Testing uthread_init...\n");
    if (uthread_init(10000) == -1) {  // 10ms quantum
        fprintf(stderr, "Init failed\n");
        return 1;
    }
    
    printf("Spawning threads...\n");
    int tid1 = uthread_spawn(sleeping_func);
    int tid2 = uthread_spawn(busy_loop_func);
    printf("Threads spawned: %d, %d\n", tid1, tid2);
    
    printf("Starting busy work...\n");
    
    // Do busy work to generate quantums
    for (volatile int i = 0; i < 100000000; i++);
    
    printf("Finished. Signals received: %d\n", signal_count);
    return 0;
}
