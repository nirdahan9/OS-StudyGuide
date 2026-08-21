#!/bin/bash

# Debug version of the extended test focusing on block/resume

cd /workspaces/Exercises/ex2/my_tests

# Create a minimal debug version
cat > test_block_issue_debug.c << 'EOF'
#include <stdio.h>
#include "../uthreads.h"

void dummy_func(void) {
    // Do nothing - just a dummy thread
    while(1) {}
}

int main() {
    printf("=== EXTENDED BLOCK/RESUME DEBUG TEST ===\n\n");
    
    uthread_init(10000);
    printf("[1] Initialized\n");
    
    // Spawn multiple threads like in the real test
    for(int i = 0; i < 10; i++) {
        int tid = uthread_spawn(dummy_func);
        printf("[spawned %d] tid=%d\n", i+1, tid);
    }
    
    printf("\n[Step 2] Now trying to block and resume a thread\n");
    
    // Try blocking thread 5
    int target_tid = 5;
    printf("Attempting: uthread_block(%d)\n", target_tid);
    int ret_block = uthread_block(target_tid);
    printf("Result: %d (expected 0)\n", ret_block);
    
    if (ret_block != 0) {
        printf("❌ FAIL: Expected 0 but got %d\n", ret_block);
        return 1;
    } else {
        printf("✓ PASS\n");
    }
    
    printf("\nAttempting: uthread_resume(%d)\n", target_tid);
    int ret_resume = uthread_resume(target_tid);
    printf("Result: %d (expected 0)\n", ret_resume);
    
    if (ret_resume != 0) {
        printf("❌ FAIL: Expected 0 but got %d\n", ret_resume);
        return 1;
    } else {
        printf("✓ PASS\n");
    }
    
    printf("\n=== END TEST ===\n");
    return 0;
}
EOF

gcc -std=gnu17 -Wall -Wextra -g ../uthreads.c ../uthread_queue.c ../jump.c test_block_issue_debug.c -o test_block_issue_debug 2>&1 | grep -v "warning: unused"

echo ""
./test_block_issue_debug
