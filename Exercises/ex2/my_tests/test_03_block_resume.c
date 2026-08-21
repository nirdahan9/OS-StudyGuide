#include <stdio.h>
#include <unistd.h>
#include "uthreads.h"

/**
 * TEST 3: uthread_block/resume Errors
 * 
 * Verifies that:
 * - block(main_tid=0) returns -1
 * - block(invalid_tid) returns -1
 * - resume(invalid_tid) returns -1
 * - block(valid_tid) returns 0
 * - resume(valid_tid) returns 0
 * 
 * From uthreads.h:
 * "It is an error to block the main thread (tid == 0)"
 * "Blocking a thread that is already BLOCKED is a no-op"
 */

void dummy_thread(void) { }

int main(void)
{
    printf("========================================\n");
    printf("   TEST 3: uthread_block/resume\n");
    printf("========================================\n\n");
    
    uthread_init(100000);
    
    // Test blocking main thread
    int ret1 = uthread_block(0);
    printf("block(main_tid=0) returned: %d\n", ret1);
    if (ret1 != -1) {
        printf("[FAIL] Expected -1, got %d\n", ret1);
        return 1;
    }
    printf("[PASS] block(main_tid=0) returns -1\n\n");
    
    // Test blocking invalid thread
    int ret2 = uthread_block(999);
    printf("block(invalid_tid=999) returned: %d\n", ret2);
    if (ret2 != -1) {
        printf("[FAIL] Expected -1, got %d\n", ret2);
        return 1;
    }
    printf("[PASS] block(invalid_tid) returns -1\n\n");
    
    // Test resume invalid thread
    int ret3 = uthread_resume(999);
    printf("resume(invalid_tid=999) returned: %d\n", ret3);
    if (ret3 != -1) {
        printf("[FAIL] Expected -1, got %d\n", ret3);
        return 1;
    }
    printf("[PASS] resume(invalid_tid) returns -1\n\n");
    
    // Spawn a thread and block it
    int tid = uthread_spawn(dummy_thread);
    printf("Spawned thread: tid=%d\n", tid);
    
    int ret4 = uthread_block(tid);
    printf("block(tid=%d) returned: %d\n", tid, ret4);
    if (ret4 != 0) {
        printf("[FAIL] Expected 0, got %d\n", ret4);
        return 1;
    }
    printf("[PASS] block(valid_tid) returns 0\n\n");
    
    // Resume it
    int ret5 = uthread_resume(tid);
    printf("resume(tid=%d) returned: %d\n", tid, ret5);
    if (ret5 != 0) {
        printf("[FAIL] Expected 0, got %d\n", ret5);
        return 1;
    }
    printf("[PASS] resume(valid_tid) returns 0\n\n");
    
    printf("========================================\n");
    printf("        ALL TESTS PASSED!\n");
    printf("========================================\n");
    
    return 0;
}
