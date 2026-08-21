#include <stdio.h>
#include "uthreads.h"

void dummy(void) {
    printf("  Dummy thread running\n");
    uthread_terminate(uthread_get_tid());
}

int main() {
    printf("Starting test\n");
    printf("Init...\n");
    uthread_init(100000);
    printf("Init done\n");
    
    printf("Spawn 1...\n");
    int tid1 = uthread_spawn(dummy);
    printf("Spawn 1 done, tid=%d\n", tid1);
    
    printf("Done!\n");
    return 0;
}
