#!/bin/bash

cd /workspaces/Exercises/ex2/my_tests

cat > test_simulate_extended.c << 'EOF'
#include <stdio.h>
#include "../uthreads.h"

void empty_thread(void) {
    // Empty thread
}

void multi_execution_thread(void) {
    int tid = uthread_get_tid();
    // Just do something minimal
}

int main() {
    printf("=== SIMULATING EXTENDED TEST SEQUENCE ===\n\n");
    
    uthread_init(10000);
    printf("[1] Initialized\n\n");
    
    // Simulate test_group_2_spawn_errors (creates more threads)
    printf("[PHASE 2] Spawning threads like in test_group_2_spawn_errors\n");
    int tid_a = uthread_spawn(empty_thread);
    int tid_b = uthread_spawn(empty_thread);
    printf("- Spawned tid_a=%d, tid_b=%d\n", tid_a, tid_b);
    
    for (int i = 0; i < 5; i++) {
        int tid = uthread_spawn(empty_thread);
        printf("- Spawned tid=%d\n", tid);
    }
    
    // Simulate test_group_2_spawn_max_threads
    printf("\n[PHASE 3] Spawning many threads like in test_group_2_spawn_max_threads\n");
    int success_count = 0;
    int fail_count = 0;
    
    // Try to fill remaining slots
    for (int i = 0; i < 100; i++) {  // Try a lot
        int tid = uthread_spawn(empty_thread);
        if (tid > 0) {
            success_count++;
        } else if (tid == -1) {
            fail_count++;
            printf("- Hit thread limit at iteration %d\n", i);
            break;
        }
    }
    printf("- Successfully spawned: %d threads\n", success_count);
    printf("- Failed to spawn: %d times\n", fail_count);
    
    // Now simulate test_group_3_block_resume
    printf("\n[PHASE 4] Starting test_group_3_block_resume\n");
    printf("\n  TEST 3.3: Block valid thread\n");
    
    int tid = uthread_spawn(multi_execution_thread);
    if (tid == -1) {
        printf("  ❌ FAIL: Could not spawn thread for test 3.3 (tid=%d)\n", tid);
        return 1;
    }
    printf("  - Spawned tid=%d\n", tid);
    
    int ret3 = uthread_block(tid);
    printf("  - uthread_block(%d) returned %d\n", tid, ret3);
    
    if (ret3 == 0) {
        printf("  ✓ PASS\n");
    } else {
        printf("  ❌ FAIL: Expected 0 but got %d\n", ret3);
    }
    
    printf("\n  TEST 3.5: Resume blocked thread\n");
    int ret5 = uthread_resume(tid);
    printf("  - uthread_resume(%d) returned %d\n", tid, ret5);
    
    if (ret5 == 0) {
        printf("  ✓ PASS\n");
    } else {
        printf("  ❌ FAIL: Expected 0 but got %d\n", ret5);
    }
    
    return 0;
}
EOF

gcc -std=gnu17 -Wall -Wextra -g ../uthreads.c ../uthread_queue.c ../jump.c test_simulate_extended.c -o test_simulate_extended 2>&1 | grep -v "warning: unused"

echo ""
./test_simulate_extended
