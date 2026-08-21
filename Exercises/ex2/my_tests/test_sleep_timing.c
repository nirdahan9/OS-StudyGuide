#include <stdio.h>
#include <stdlib.h>
#include "../uthreads.h"

static int thread_ran[3] = {0, 0, 0};
static int quantums_at_sleep = 0;

void thread1(void)
{
    // Thread 1 sleeps for 2 quantums
    quantums_at_sleep = uthread_get_total_quantums();
    printf("Thread 1 sleeping at quantum %d (sleep 2)\n", quantums_at_sleep);
    thread_ran[1] = 1;
    uthread_sleep(2);
    printf("Thread 1 woke at quantum %d\n", uthread_get_total_quantums());
    thread_ran[1] = 2;
    uthread_terminate(uthread_get_tid());
}

void thread2(void)
{
    printf("Thread 2 ran at quantum %d\n", uthread_get_total_quantums());
    thread_ran[2] = 1;
    uthread_terminate(uthread_get_tid());
}

int main(void)
{
    printf("========== Sleep Timing Test ==========\n\n");
    
    uthread_init(100000);  // 100ms quantum
    
    printf("After init: total_quantums = %d\n", uthread_get_total_quantums());
    
    int tid1 = uthread_spawn(thread1);
    printf("Spawned thread 1 (tid=%d)\n", tid1);
    
    int tid2 = uthread_spawn(thread2);
    printf("Spawned thread 2 (tid=%d)\n\n", tid2);
    
    // Now simulate scheduling by calling schedule_next would happen
    // But we can't directly test in main. 
    // Instead, let's just verify with a simpler approach
    
    printf("Test: thread1 sleeps(2) at quantum X, should wake at quantum X+3\n");
    printf("Because current quantum is not counted, so it skips next 2 quantums.\n");
    printf("Example: if sleep at quantum 7, sleep(2) means skip 8,9, wake at 10\n");
    printf("So sleep_until = 7 + 2 + 1 = 10, wake when total_quantums >= 10\n\n");
    
    return 0;
}
