#include <stdio.h>
#include <unistd.h>
#include "uthreads.h"

void thread_func(void)
{
    printf("Thread running: tid=%d\n", uthread_get_tid());
    uthread_terminate(uthread_get_tid());
}

int main(void)
{
    printf("Before init\n");
    fflush(stdout);
    
    int ret = uthread_init(100000);
    printf("Init returned: %d\n", ret);
    fflush(stdout);
    
    printf("Spawning thread\n");
    fflush(stdout);
    
    int tid = uthread_spawn(thread_func);
    printf("Spawned thread: tid=%d\n", tid);
    fflush(stdout);
    
    printf("Main thread: tid=%d, total=%d\n", uthread_get_tid(), uthread_get_total_quantums());
    fflush(stdout);
    
    return 0;
}
