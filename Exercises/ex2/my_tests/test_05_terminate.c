#include <stdio.h>
#include <unistd.h>
#include "uthreads.h"

/**
 * TEST 5: uthread_terminate Errors
 * 
 * Verifies that:
 * - terminate(invalid_tid) returns -1
 * - get_quantums on terminated thread returns -1
 * 
 * From uthreads.h:
 * "It is an error if no thread with the given tid exists"
 * "An error is returned if no thread with the given tid exists"
 */

void dummy_thread(void) { }

int main(void)
{
    printf("========================================\n");
    printf("   TEST 5: uthread_terminate\n");
    printf("========================================\n\n");
    
    uthread_init(100000);
    
    // Test terminating non-existent thread
    int ret1 = uthread_terminate(999);
    printf("terminate(invalid_tid=999) returned: %d\n", ret1);
    if (ret1 != -1) {
        printf("[FAIL] Expected -1, got %d\n", ret1);
        return 1;
    }
    printf("[PASS] terminate(invalid_tid) returns -1\n\n");
    
    // Spawn and terminate a thread
    int tid = uthread_spawn(dummy_thread);
    printf("Spawned thread: tid=%d\n", tid);
    
    int ret2 = uthread_terminate(tid);
    printf("terminate(tid=%d) returned: %d\n", tid, ret2);
    if (ret2 != 0) {
        printf("[FAIL] Expected 0, got %d\n", ret2);
        return 1;
    }
    printf("[PASS] terminate(valid_tid) returns 0\n\n");
    
    // Get quantums of terminated thread
    int quants = uthread_get_quantums(tid);
    printf("get_quantums(terminated_tid=%d) returned: %d\n", tid, quants);
    if (quants != -1) {
        printf("[FAIL] Expected -1, got %d\n", quants);
        return 1;
    }
    printf("[PASS] get_quantums on terminated thread returns -1\n\n");
    
    printf("========================================\n");
    printf("        ALL TESTS PASSED!\n");
    printf("========================================\n");
    
    return 0;
}
