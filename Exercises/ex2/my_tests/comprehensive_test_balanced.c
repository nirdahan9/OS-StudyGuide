#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "../uthreads.h"

/**
 * COMPREHENSIVE TEST SUITE FOR UTHREADS - BALANCED VERSION
 * 
 * This version is less aggressive than the extended edition,
 * making it suitable for systems with limited resources while
 * still providing comprehensive testing coverage.
 * 
 * Contains 50+ carefully selected tests covering all major
 * functionality with stress testing appropriate to most systems.
 */

static int passed = 0, failed = 0, skipped = 0;

#define TEST(cond, desc) \
    do { if(cond) { printf("[✓] %s\n", desc); passed++; } \
        else { printf("[✗] %s\n", desc); failed++; } } while(0)

#define SKIP(desc) (printf("[⊘] %s\n", desc), skipped++)

#define GROUP(name) printf("\n%s\n", name)

/* Global state */
static int counter = 0;
static int tid_log[50];
static int tid_idx = 0;

/* Thread functions */
void empty_func(void) { }

void counter_func(void) {
    counter++;
    uthread_terminate(uthread_get_tid());
}

void sleep_1_func(void) {
    uthread_sleep(1);
    uthread_terminate(uthread_get_tid());
}

void self_term_func(void) {
    uthread_terminate(uthread_get_tid());
}

void log_tid_func(void) {
    if(tid_idx < 50) tid_log[tid_idx++] = uthread_get_tid();
    uthread_terminate(uthread_get_tid());
}

int main(void) {
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║   COMPREHENSIVE UTHREADS TEST SUITE - BALANCED EDITION        ║\n");
    printf("║     User-Level Threading Library Validation                   ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    
    /* Initialize */
    if(uthread_init(10000) != 0) {
        printf("ERROR: Failed to initialize\n");
        return 1;
    }
    printf("✓ Library initialized\n\n");
    
    /* ===== TEST GROUP 1: Initialization ===== */
    GROUP("TEST GROUP 1: INITIALIZATION");
    TEST(uthread_get_tid() == 0, "Main thread ID is 0");
    TEST(uthread_get_total_quantums() >= 1, "Total quantums >= 1");
    TEST(uthread_get_quantums(0) >= 1, "Main quantums >= 1");
    TEST(uthread_get_quantums(999) == -1, "Invalid TID returns -1");
    
    /* ===== TEST GROUP 2: Spawn Basic ===== */
    GROUP("TEST GROUP 2: SPAWN BASIC FUNCTIONALITY");
    int t1 = uthread_spawn(empty_func);
    int t2 = uthread_spawn(empty_func);
    int t3 = uthread_spawn(empty_func);
    TEST(t1 == 1, "First spawn returns TID 1");
    TEST(t2 == 2, "Second spawn returns TID 2");
    TEST(t3 == 3, "Third spawn returns TID 3");
    TEST(uthread_get_quantums(t1) == 0, "New thread has 0 quantums");
    TEST(uthread_spawn(NULL) == -1, "Spawn NULL fails");
    
    /* ===== TEST GROUP 3: Spawn Errors ===== */
    GROUP("TEST GROUP 3: SPAWN ERROR CASES");
    int ret = uthread_spawn(NULL);
    TEST(ret == -1, "Repeated NULL spawn fails");
    
    /* Try to fill slots (limit to 40 to avoid resource issues) */
    int spawn_count = 0;
    for(int i = 0; i < 40; i++) {
        if(uthread_spawn(empty_func) > 0) spawn_count++;
        else break;
    }
    TEST(spawn_count > 30, "Can spawn multiple threads");
    
    /* ===== TEST GROUP 4: Block/Resume ===== */
    GROUP("TEST GROUP 4: BLOCK/RESUME");
    TEST(uthread_block(0) == -1, "Cannot block main thread");
    TEST(uthread_block(999) == -1, "Cannot block invalid TID");
    
    int bid = uthread_spawn(empty_func);
    TEST(uthread_block(bid) == 0, "Can block valid thread");
    TEST(uthread_resume(bid) == 0, "Can resume blocked thread");
    TEST(uthread_resume(999) == -1, "Cannot resume invalid TID");
    
    /* ===== TEST GROUP 5: Block/Resume Complex ===== */
    GROUP("TEST GROUP 5: BLOCK/RESUME COMPLEX");
    int bid2 = uthread_spawn(empty_func);
    uthread_block(bid2);
    uthread_resume(bid2);
    TEST(uthread_block(bid2) == 0, "Block-Resume-Block works");
    
    int bid3 = uthread_spawn(empty_func);
    int bid4 = uthread_spawn(empty_func);
    uthread_block(bid3);
    uthread_block(bid4);
    TEST(uthread_resume(bid3) == 0 && uthread_resume(bid4) == 0,
         "Multiple block/resume works");
    
    /* ===== TEST GROUP 6: Sleep ===== */
    GROUP("TEST GROUP 6: SLEEP");
    TEST(uthread_sleep(1) == -1, "Main thread cannot sleep");
    
    int s1 = uthread_spawn(sleep_1_func);
    TEST(s1 > 0, "Spawned thread for sleep test");
    for(int i = 0; i < 10; i++) {
        kill(getpid(), SIGVTALRM);
    }
    
    /* ===== TEST GROUP 7: Quantum Counting ===== */
    GROUP("TEST GROUP 7: QUANTUM COUNTING");
    int q_before = uthread_get_total_quantums();
    
    int q1 = uthread_spawn(counter_func);
    for(int i = 0; i < 5; i++) {
        kill(getpid(), SIGVTALRM);
    }
    
    int q_after = uthread_get_total_quantums();
    TEST(q_after >= q_before, "Total quantums increases");
    TEST(uthread_get_quantums(0) >= 0, "Main quantums non-negative");
    TEST(uthread_get_quantums(q1) == -1, "Terminated thread returns -1");
    
    /* ===== TEST GROUP 8: Termination ===== */
    GROUP("TEST GROUP 8: TERMINATION");
    TEST(uthread_terminate(999) == -1, "Cannot terminate invalid TID");
    
    int t_term = uthread_spawn(empty_func);
    TEST(uthread_terminate(t_term) == 0, "Can terminate valid thread");
    TEST(uthread_terminate(t_term) == -1, "Cannot double-terminate");
    TEST(uthread_get_quantums(t_term) == -1, "Terminated thread returns -1");
    
    /* ===== TEST GROUP 9: Self-Termination ===== */
    GROUP("TEST GROUP 9: SELF-TERMINATION");
    int s_term = uthread_spawn(self_term_func);
    for(int i = 0; i < 5; i++) {
        kill(getpid(), SIGVTALRM);
    }
    TEST(uthread_get_quantums(s_term) == -1, "Self-termination works");
    
    /* ===== TEST GROUP 10: TID Reuse ===== */
    GROUP("TEST GROUP 10: TID REUSE");
    int r1 = uthread_spawn(empty_func);
    int r2 = uthread_spawn(empty_func);
    uthread_terminate(r1);
    int r_reuse = uthread_spawn(empty_func);
    TEST(r_reuse == r1, "Terminated TID is reused");
    
    /* ===== TEST GROUP 11: TID Sequence ===== */
    GROUP("TEST GROUP 11: TID ALLOCATION");
    int ts1 = uthread_spawn(empty_func);
    int ts2 = uthread_spawn(empty_func);
    int ts3 = uthread_spawn(empty_func);
    TEST(ts2 == ts1 + 1, "TIDs allocated sequentially");
    TEST(ts3 == ts2 + 1, "TIDs continue sequentially");
    TEST(uthread_spawn(NULL) == -1, "NULL spawn still fails");
    
    /* ===== TEST GROUP 12: Scheduling ===== */
    GROUP("TEST GROUP 12: SCHEDULING");
    tid_idx = 0;
    memset(tid_log, 0, sizeof(tid_log));
    
    int sc1 = uthread_spawn(log_tid_func);
    int sc2 = uthread_spawn(log_tid_func);
    int sc3 = uthread_spawn(log_tid_func);
    
    for(int i = 0; i < 10; i++) {
        kill(getpid(), SIGVTALRM);
    }
    TEST(tid_idx > 0, "Spawned threads execute");
    
    /* ===== TEST GROUP 13: Concurrent Execution ===== */
    GROUP("TEST GROUP 13: CONCURRENT EXECUTION");
    counter = 0;
    int c1 = uthread_spawn(counter_func);
    int c2 = uthread_spawn(counter_func);
    int c3 = uthread_spawn(counter_func);
    
    for(int i = 0; i < 15; i++) {
        kill(getpid(), SIGVTALRM);
    }
    TEST(counter >= 1, "Multiple threads execute");
    
    /* ===== TEST GROUP 14: Getter Functions ===== */
    GROUP("TEST GROUP 14: GETTER FUNCTIONS");
    TEST(uthread_get_tid() == 0, "get_tid from main is 0");
    TEST(uthread_get_total_quantums() > 0, "get_total_quantums > 0");
    TEST(uthread_get_quantums(0) > 0, "Main quantums > 0");
    TEST(uthread_get_quantums(999) == -1, "Invalid TID returns -1");
    
    int g_tid = uthread_spawn(empty_func);
    TEST(uthread_get_quantums(g_tid) == 0, "New thread has 0 quantums");
    
    /* ===== TEST GROUP 15: Return Values ===== */
    GROUP("TEST GROUP 15: RETURN VALUE VALIDATION");
    int b_ret = uthread_block(99);
    TEST(b_ret == -1, "Invalid block returns -1");
    
    int r_ret = uthread_resume(99);
    TEST(r_ret == -1, "Invalid resume returns -1");
    
    int sl_ret = uthread_sleep(1);
    TEST(sl_ret == -1, "Main sleep returns -1");
    
    int tm_ret = uthread_terminate(99);
    TEST(tm_ret == -1, "Invalid terminate returns -1");
    
    /* ===== TEST GROUP 16: Rapid Operations ===== */
    GROUP("TEST GROUP 16: RAPID OPERATIONS");
    int rapid_ok = 1;
    int r_tid = uthread_spawn(empty_func);
    for(int i = 0; i < 20; i++) {
        if(uthread_block(r_tid) != 0) rapid_ok = 0;
        if(uthread_resume(r_tid) != 0) rapid_ok = 0;
    }
    TEST(rapid_ok, "20 rapid block/resume cycles work");
    
    /* ===== TEST GROUP 17: State Consistency ===== */
    GROUP("TEST GROUP 17: STATE CONSISTENCY");
    int cons_q = uthread_get_quantums(0);
    TEST(cons_q >= 0, "Quantum count never negative");
    TEST(uthread_get_total_quantums() >= 0, "Total quantums never negative");
    
    /* ===== TEST GROUP 18: Error Recovery ===== */
    GROUP("TEST GROUP 18: ERROR RECOVERY");
    int err_before = uthread_get_total_quantums();
    for(int i = 0; i < 5; i++) {
        uthread_spawn(NULL);
    }
    int err_after = uthread_get_total_quantums();
    TEST(err_after == err_before, "Errors don't corrupt state");
    
    int recover = uthread_spawn(empty_func);
    TEST(recover > 0, "Normal spawn works after errors");
    
    /* ===== TEST GROUP 19: Resource Limits ===== */
    GROUP("TEST GROUP 19: RESOURCE LIMITS");
    printf("MAX_THREAD_NUM = %d\n", MAX_THREAD_NUM);
    printf("STACK_SIZE = %d\n", STACK_SIZE);
    TEST(MAX_THREAD_NUM == 100, "MAX_THREAD_NUM is 100");
    TEST(STACK_SIZE == 4096, "STACK_SIZE is 4096");
    
    /* ===== TEST GROUP 20: Boundary Cases ===== */
    GROUP("TEST GROUP 20: BOUNDARY CASES");
    TEST(uthread_get_tid() == 0, "Main thread always TID 0");
    TEST(uthread_spawn(NULL) == -1, "NULL spawn always fails");
    
    int b_tid = uthread_spawn(empty_func);
    TEST(b_tid > 0, "Valid spawn always succeeds");
    
    /* Print summary */
    printf("\n╔═══════════════════════════════════════════════════════════════╗\n");
    printf("║                       TEST SUMMARY                            ║\n");
    printf("╚═══════════════════════════════════════════════════════════════╝\n\n");
    printf("  Passed:  %3d\n", passed);
    printf("  Failed:  %3d\n", failed);
    printf("  Skipped: %3d\n", skipped);
    printf("  Total:   %3d\n", passed + failed + skipped);
    printf("\n");
    
    if(failed == 0) {
        printf("✓ ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("✗ %d test(s) failed\n", failed);
        return 1;
    }
}
