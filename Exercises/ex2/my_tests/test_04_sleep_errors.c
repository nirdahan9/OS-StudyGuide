#include <stdio.h>
#include <unistd.h>
#include "uthreads.h"

/**
 * TEST 4: uthread_sleep Error Cases
 * 
 * Verifies that:
 * - uthread_sleep from main thread returns -1
 * 
 * From uthreads.h:
 * "It is an error for the main thread (tid == 0) to call this function"
 */

int main(void)
{
    printf("========================================\n");
    printf("   TEST 4: uthread_sleep Errors\n");
    printf("========================================\n\n");
    
    uthread_init(100000);
    
    printf("Calling uthread_sleep(1) from main thread...\n");
    int ret = uthread_sleep(1);
    printf("uthread_sleep(1) returned: %d\n", ret);
    
    if (ret != -1) {
        printf("[FAIL] Expected -1, got %d\n", ret);
        return 1;
    }
    printf("[PASS] uthread_sleep from main thread returns -1\n\n");
    
    printf("========================================\n");
    printf("        ALL TESTS PASSED!\n");
    printf("========================================\n");
    
    return 0;
}
