#include "uthread_queue.h"
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "uthreads.h"


static int next_in_queue[MAX_THREAD_NUM];
static int ready_queue_head = -1;
static int ready_queue_tail = -1;

// Initialize empty queue
void queue_init(void)
{
    ready_queue_head = -1;
    ready_queue_tail = -1;
    for (int i = 0; i < MAX_THREAD_NUM; i++) {
        next_in_queue[i] = -1;
    }
}

// Add thread to back of queue
void enqueue_ready(int tid)
{
    next_in_queue[tid] = -1;
    
    if (ready_queue_head == -1) { // Queue is empty
        ready_queue_head = tid;
        ready_queue_tail = tid;
    } else {
        next_in_queue[ready_queue_tail] = tid;
        ready_queue_tail = tid;
    }
}

int dequeue_ready(void)
{
    if (ready_queue_head == -1) { // Queue is empty
        return -1;
    }
    
    int tid = ready_queue_head;
    ready_queue_head = next_in_queue[tid];
    
    if (ready_queue_head == -1) { // Queue is now empty
        ready_queue_tail = -1;
    }
    
    next_in_queue[tid] = -1;  // Reset node
    return tid;
}

void remove_from_ready(int tid)
{
    if (ready_queue_head == -1) { // Queue is empty, nothing to remove
        return;
    }
    
    // If the thread to remove is at the head of the queue
    if (ready_queue_head == tid) {
        ready_queue_head = next_in_queue[tid];
        if (ready_queue_head == -1) {
            ready_queue_tail = -1;
        }
        next_in_queue[tid] = -1;
        return;
    }
    
    // Search for tid in the queue
    int current = ready_queue_head;
    while (current != -1) {
        int next = next_in_queue[current];
        
        // If next is the thread to remove, unlink it
        if (next == tid) {
            next_in_queue[current] = next_in_queue[tid];
            if (next_in_queue[tid] == -1) {
                ready_queue_tail = current;
            }
            next_in_queue[tid] = -1;
            return;
        }
        current = next;
    }
}

bool is_in_ready_queue(int tid)
{
    int current = ready_queue_head;
    while (current != -1) {
        if (current == tid) {
            return true;
        }
        current = next_in_queue[current];
    }
    return false;
}

/* ===================================================================== */
/*                       Signal Management                               */
/* ===================================================================== */

extern int current_tid;  // from uthreads.c

static sigset_t set;
static sigset_t oldset_per_thread[MAX_THREAD_NUM];

// Block SIGVTALRM and save mask
void mask_signal(void)
{
    // Block SIGVTALRM and save the old signal mask for the current thread
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);
    
    if (sigprocmask(SIG_BLOCK, &set, &oldset_per_thread[current_tid]) < 0) {
        fprintf(stderr, "system error: masking failed\n");
        exit(1);
    }
}

// Restore saved signal mask
void unmask_signal(void)
{
    if (sigprocmask(SIG_SETMASK, &oldset_per_thread[current_tid], NULL) < 0) {
        fprintf(stderr, "system error: masking failed\n");
        exit(1);
    }
}
