#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include "uthreads.h"

/**
 * TEST 1: uthread_init Validation
 * 
 * Verifies that:
 * - init returns 0 on success
 * - total_quantums == 1 immediately after init
 * - main thread quantums == 1 after init  
 * - main thread tid == 0
 * 
 * From uthreads.h:
 * "The count starts at 1 immediately after uthread_init"
 */

int main(void)
{
    printf("========================================\n");
    printf("   TEST 1: uthread_init Validation\n");
    printf("========================================\n\n");
    
    int ret = uthread_init(100000);
    printf("uthread_init(100000) returned: %d\n", ret);
    
    if (ret != 0) {
        printf("[FAIL] Expected 0, got %d\n", ret);
        return 1;
    }
    printf("[PASS] init returns 0\n\n");
    
    int total = uthread_get_total_quantums();
    printf("total_quantums immediately after init: %d\n", total);
    
    if (total != 1) {
        printf("[FAIL] Expected 1, got %d\n", total);
        return 1;
    }
    printf("[PASS] total_quantums == 1\n\n");
    
    int main_quants = uthread_get_quantums(0);
    printf("main thread quantums: %d\n", main_quants);
    
    if (main_quants != 1) {
        printf("[FAIL] Expected 1, got %d\n", main_quants);
        return 1;
    }
    printf("[PASS] main thread quantums == 1\n\n");
    
    int tid = uthread_get_tid();
    printf("main thread tid: %d\n", tid);
    
    if (tid != 0) {
        printf("[FAIL] Expected 0, got %d\n", tid);
        return 1;
    }
    printf("[PASS] main thread tid == 0\n\n");
    
    printf("========================================\n");
    printf("        ALL TESTS PASSED!\n");
    printf("========================================\n");
    
    return 0;
}
