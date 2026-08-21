#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <stdbool.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include "jump.h"
#include "uthreads.h"
#include "uthread_queue.h"

/* ===================================================================== */
/*                           Global Variables                            */
/* ===================================================================== */

static thread_t threads[MAX_THREAD_NUM];
static char stacks[MAX_THREAD_NUM][STACK_SIZE];
int current_tid = -1;  // Non-static so uthread_queue.c can access it
static int total_quantums = 0;
static int g_quantum_usecs = 0;
static bool is_blocked[MAX_THREAD_NUM]; // true if thread is explicitly blocked
static thread_entry_point actual_entry_points[MAX_THREAD_NUM]; // Store actual entry points

// Entry point for all threads: saves context and calls entry function
static void thread_wrapper(void)
{
    int tid = current_tid;
    thread_entry_point entry = actual_entry_points[tid];
    
    if (sigsetjmp(threads[tid].env, 1) == 0) {
        entry();
    }
    
    uthread_terminate(tid);
}

/* ===================================================================== */
/*                            Public Functions                           */
/* ===================================================================== */

// Initialize library, set up main thread and timer
int uthread_init(int quantum_usecs)
{
    // Edge case: quantum_usecs must be positive
    if (quantum_usecs <= 0) {
        fprintf(stderr, "thread library error: quantum_usecs must be positive\n");
        return -1;
    }
    
    g_quantum_usecs = quantum_usecs;
    total_quantums = 1;
    queue_init();
    
    // Initialize all threads as unused
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        threads[i].state = THREAD_UNUSED;
        is_blocked[i] = false;
    }
    
    // Set up main thread
    threads[0].tid = 0;
    threads[0].state = THREAD_RUNNING;
    threads[0].quantums = 1; // Main thread already executed 1 quantum
    threads[0].sleep_until = 0;
    threads[0].entry = NULL;
    is_blocked[0] = false;
    
    // Save main thread's context for later restoration via siglongjmp. Reached when returning from another thread.
    if (sigsetjmp(threads[0].env, 1) != 0) {
        return 0;
    }
    
    current_tid = 0;
    
    // Set up timer handler and timer
    struct sigaction sa;
    sa.sa_handler = timer_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGVTALRM, &sa, NULL) == -1) {
        fprintf(stderr, "system error: sigaction failed\n");
        exit(1);
    }
    
    // Set up timer
    struct itimerval timer;
    timer.it_value.tv_sec = quantum_usecs / 1000000;
    timer.it_value.tv_usec = quantum_usecs % 1000000;
    timer.it_interval.tv_sec = quantum_usecs / 1000000;
    timer.it_interval.tv_usec = quantum_usecs % 1000000;
    
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1) {
        fprintf(stderr, "system error: setitimer failed\n");
        exit(1);
    }
    
    return 0;
}

int uthread_spawn(thread_entry_point entry_point)
{
    if (!entry_point) {
        fprintf(stderr, "thread library error: entry_point cannot be NULL\n");
        return -1;
    }
    
    mask_signal();
    
    int slot_tid = -1;
    for (int i = 1; i < MAX_THREAD_NUM; i++) { 
        if (threads[i].state == THREAD_UNUSED || threads[i].state == THREAD_TERMINATED) {
            setup_thread(i, stacks[i], entry_point);
            threads[i].state = THREAD_READY;
            is_blocked[i] = false;
            enqueue_ready(i);
            slot_tid = i;
            break;
        }
    }
    
    if (slot_tid == -1) {
        fprintf(stderr, "thread library error: no available thread slots\n");
    }
    
    unmask_signal();
    return slot_tid;
}

int uthread_terminate(int tid)
{
    // Edge cases: invalid tid or non-existent thread
    if (tid < 0 || tid >= MAX_THREAD_NUM) {
        fprintf(stderr, "thread library error: invalid thread ID\n");
        return -1;
    }
    if (threads[tid].state == THREAD_UNUSED || threads[tid].state == THREAD_TERMINATED) {
        fprintf(stderr, "thread library error: thread does not exist\n");
        return -1;
    }
    
    mask_signal();
    
    // If terminating the main thread, terminate the entire process
    if (tid == 0) {
        unmask_signal();
        exit(0);
    }
    
    // Remove from READY queue if present
    remove_from_ready(tid);
    
    // Mark the thread as terminated
    threads[tid].state = THREAD_TERMINATED;
    is_blocked[tid] = false;
    
    // If terminating the current thread, switch to another thread
    if (tid == current_tid) {
        schedule_next();
    }
    
    unmask_signal();
    return 0;
}

int uthread_block(int tid)
{
    // Edge cases: invalid tid, non-existent thread, or main thread
    if (tid == 0) {
        fprintf(stderr, "thread library error: cannot block main thread\n");
        return -1;
    }    
    if (tid < 0 || tid >= MAX_THREAD_NUM) {
        fprintf(stderr, "thread library error: invalid thread ID\n");
        return -1;
    }
    if (threads[tid].state == THREAD_UNUSED || threads[tid].state == THREAD_TERMINATED) {
        fprintf(stderr, "thread library error: thread does not exist\n");
        return -1;
    }
    
    mask_signal();
    
    // Mark as blocked
    if (!is_blocked[tid]) {
        is_blocked[tid] = true;
        if (threads[tid].state == THREAD_READY) {
            remove_from_ready(tid);
            threads[tid].state = THREAD_BLOCKED;
        }
    }
    
    // If the thread is blocking itself, schedule the next thread
    if (tid == current_tid) {
        schedule_next();
    }
    
    unmask_signal();
    return 0;
}

int uthread_resume(int tid)
{
    // Edge cases: invalid tid or non-existent thread
    if (tid < 0 || tid >= MAX_THREAD_NUM) {
        fprintf(stderr, "thread library error: invalid thread ID\n");
        return -1;
    }
    if (threads[tid].state == THREAD_UNUSED || threads[tid].state == THREAD_TERMINATED) {
        fprintf(stderr, "thread library error: thread does not exist\n");
        return -1;
    }
    
    mask_signal();
    
    // Clear the explicit blocking flag
    if (is_blocked[tid]) {
        is_blocked[tid] = false;
        
        // If thread is BLOCKED and sleep has expired, move to READY
        if (threads[tid].state == THREAD_BLOCKED && 
            (threads[tid].sleep_until == 0 || total_quantums > threads[tid].sleep_until)) {
            threads[tid].state = THREAD_READY;
            threads[tid].sleep_until = 0;
            enqueue_ready(tid);
        }
    }
    
    unmask_signal();
    return 0;
}

int uthread_sleep(int num_quantums)
{
    
    // Edge case: main thread cannot sleep & num_quantums must be positive
    if (current_tid == 0) {
        fprintf(stderr, "thread library error: main thread cannot sleep\n");
        return -1;
    }
    if (num_quantums <= 0) {
        fprintf(stderr, "thread library error: num_quantums must be positive\n");
        return -1;
    }
    
    mask_signal();
    
    // Set sleep_until to wake up after num_quantums from now (add 1 to wake after the specified quantums have passed)
    threads[current_tid].sleep_until = total_quantums + num_quantums + 1;
    threads[current_tid].state = THREAD_BLOCKED;
    schedule_next();
    
    unmask_signal();
    return 0;
}

int uthread_get_tid()
{
    return current_tid;
}

int uthread_get_total_quantums()
{
    return total_quantums;
}

int uthread_get_quantums(int tid)
{
    // Edge cases: invalid tid or non-existent thread
    if (tid < 0 || tid >= MAX_THREAD_NUM) {
        return -1;
    }
    if (threads[tid].state == THREAD_UNUSED || threads[tid].state == THREAD_TERMINATED) {
        return -1;
    }
    return threads[tid].quantums;
}

/* ===================================================================== */
/*              Internal Helper Functions and Structures                 */
/* ===================================================================== */

void schedule_next(void)
{
    // Increment total quantum count - this happens every time the timer fires
    total_quantums++;
    
    // Wake sleeping threads
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        if (threads[i].state == THREAD_BLOCKED && 
            threads[i].sleep_until != 0 && 
            total_quantums >= threads[i].sleep_until &&
            !is_blocked[i]) {
            threads[i].state = THREAD_READY;
            threads[i].sleep_until = 0;
            enqueue_ready(i);
        }
    }
    
    // Get the next READY thread from the front of the queue
    int next_tid = dequeue_ready();
    
    
    // If no ready threads, continue running the current thread
    if (next_tid == -1) {
        return;
    }
    
    
    // Switch context to the next thread
    int saved_current = current_tid;
    current_tid = next_tid;
    context_switch(&threads[saved_current], &threads[next_tid]);
}

void context_switch(thread_t *current, thread_t *next)
{
    // Increment quantums for the thread that is about to run
    next->quantums++;
    
    
    // If the current thread is still running, move it to READY state and enqueue it if not blocked
    if (current->state == THREAD_RUNNING) {
        current->state = THREAD_READY;
        if (!is_blocked[current->tid]) {
            enqueue_ready(current->tid);
        }
    }
    
    
    // Transition next thread to RUNNING
    next->state = THREAD_RUNNING;
    
    // Reset the timer for the new quantum
    struct itimerval timer;
    timer.it_value.tv_sec = g_quantum_usecs / 1000000;
    timer.it_value.tv_usec = g_quantum_usecs % 1000000;
    timer.it_interval.tv_sec = g_quantum_usecs / 1000000;
    timer.it_interval.tv_usec = g_quantum_usecs % 1000000;
    
    
    // Check setitimer failiure
    if (setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1) {
        fprintf(stderr, "system error: setitimer failed\n");
        exit(1);
    }

    if (sigsetjmp(current->env, 1) == 0) {
        siglongjmp(next->env, 1);
    }
}

// Signal handler: schedule next thread
void timer_handler(int signum)
{
    (void)signum;
    schedule_next();
}

// Initialize thread TCB and jump buffer
void setup_thread(int tid, char *stack, thread_entry_point entry_point)
{
    threads[tid].tid = tid;
    threads[tid].state = THREAD_UNUSED; // Will be set to READY in uthread_spawn
    threads[tid].quantums = 0;
    threads[tid].sleep_until = 0;
    threads[tid].entry = entry_point;
    is_blocked[tid] = false;
    
    actual_entry_points[tid] = entry_point;
    setup_jmpbuff(&threads[tid].env, stack, STACK_SIZE, thread_wrapper);
}
