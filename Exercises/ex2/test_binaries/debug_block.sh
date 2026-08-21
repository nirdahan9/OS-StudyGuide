#!/bin/bash

# Test the block/resume functionality

cd /workspaces/Exercises/ex2

cat > test_block_debug.c << 'EOF'
#include <stdio.h>
#include "uthreads.h"

void dummy_func(void) {
    // Do nothing
}

int main() {
    printf("=== BLOCK/RESUME DEBUG TEST ===\n\n");
    
    uthread_init(10000);
    printf("[1] Initialized\n");
    
    int tid = uthread_spawn(dummy_func);
    printf("[2] Spawned thread %d\n", tid);
    
    int ret_block = uthread_block(tid);
    printf("[3] uthread_block(%d) returned: %d (expected 0)\n", tid, ret_block);
    
    if (ret_block != 0) {
        printf("    ❌ FAIL: Expected 0 but got %d\n", ret_block);
    } else {
        printf("    ✓ PASS\n");
    }
    
    int ret_resume = uthread_resume(tid);
    printf("[4] uthread_resume(%d) returned: %d (expected 0)\n", tid, ret_resume);
    
    if (ret_resume != 0) {
        printf("    ❌ FAIL: Expected 0 but got %d\n", ret_resume);
    } else {
        printf("    ✓ PASS\n");
    }
    
    int ret_block2 = uthread_block(tid);
    printf("[5] uthread_block(%d) returned: %d (expected 0)\n", tid, ret_block2);
    
    if (ret_block2 != 0) {
        printf("    ❌ FAIL: Expected 0 but got %d\n", ret_block2);
    } else {
        printf("    ✓ PASS\n");
    }
    
    printf("\n=== END TEST ===\n");
    return 0;
}
EOF

gcc -std=gnu17 -Wall -Wextra -g uthreads.c uthread_queue.c jump.c test_block_debug.c -o test_block_debug 2>&1

echo "Compiling..."
echo ""

./test_block_debug
