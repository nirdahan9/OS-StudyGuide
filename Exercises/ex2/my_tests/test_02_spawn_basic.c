#include <stdio.h>
#include <unistd.h>
#include "uthreads.h"

/**
 * TEST 2: uthread_spawn Basic Functionality
 * 
 * Verifies that:
 * - first spawn returns tid 1
 * - newly spawned thread has quantums == 0
 * - second spawn returns tid 2
 * - spawn(NULL) returns -1
 * 
 * From uthreads.h:
 * "returns the new thread's ID; on failure, returns -1"
 * "Calling this function with a NULL entry_point is an error"
 */

void dummy_thread(void) { }

int main(void)
{
    printf("========================================\n");
    printf("   TEST 2: uthread_spawn Basic\n");
    printf("========================================\n\n");
    
    uthread_init(100000);
    
    int tid1 = uthread_spawn(dummy_thread);
    printf("First spawn returned: %d\n", tid1);
    if (tid1 != 1) {
        printf("[FAIL] Expected tid 1, got %d\n", tid1);
        return 1;
    }
    printf("[PASS] first spawn returns tid 1\n\n");
    
    int quants1 = uthread_get_quantums(tid1);
    printf("Newly spawned thread quantums: %d\n", quants1);
    if (quants1 != 0) {
        printf("[FAIL] Expected 0, got %d\n", quants1);
        return 1;
    }
    printf("[PASS] newly spawned thread has quantums == 0\n\n");
    
    int tid2 = uthread_spawn(dummy_thread);
    printf("Second spawn returned: %d\n", tid2);
    if (tid2 != 2) {
        printf("[FAIL] Expected tid 2, got %d\n", tid2);
        return 1;
    }
    printf("[PASS] second spawn returns tid 2\n\n");
    
    int null_ret = uthread_spawn(NULL);
    printf("spawn(NULL) returned: %d\n", null_ret);
    if (null_ret != -1) {
        printf("[FAIL] Expected -1, got %d\n", null_ret);
        return 1;
    }
    printf("[PASS] spawn(NULL) returns -1\n\n");
    
    printf("========================================\n");
    printf("        ALL TESTS PASSED!\n");
    printf("========================================\n");
    
    return 0;
}
