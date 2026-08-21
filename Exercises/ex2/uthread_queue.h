#ifndef _UTHREAD_QUEUE_H
#define _UTHREAD_QUEUE_H

#include <stdbool.h>

/*
 * Queue and signal management helpers for the user-level threading library.
 * Implements a linked-list READY queue using index-based nodes (intrusive list pattern).
 * Provides O(1) enqueue, dequeue, and remove operations.
 */

/**
 * @brief Initialize the READY queue (called by uthread_init).
 */
void queue_init(void);

/**
 * @brief Add a thread to the back of the READY queue.
 * @param tid Thread ID to enqueue.
 */
void enqueue_ready(int tid);

/**
 * @brief Remove and return the first thread from the READY queue.
 * @return Thread ID at front of queue, or -1 if queue is empty.
 */
int dequeue_ready(void);

/**
 * @brief Remove a specific thread from the READY queue (may be anywhere in the queue).
 * @param tid Thread ID to remove.
 */
void remove_from_ready(int tid);

/**
 * @brief Check if a thread is currently in the READY queue.
 * @param tid Thread ID to check.
 * @return true if thread is in queue, false otherwise.
 */
bool is_in_ready_queue(int tid);

/**
 * @brief Block signals during critical sections.
 * Masks SIGVTALRM to protect shared state modifications.
 */
void mask_signal(void);

/**
 * @brief Restore signal mask after critical section.
 * Unmasks SIGVTALRM to resume normal operation.
 */
void unmask_signal(void);

#endif /* _UTHREAD_QUEUE_H */
