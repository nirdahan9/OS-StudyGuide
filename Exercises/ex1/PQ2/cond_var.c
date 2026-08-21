#include <stdatomic.h>
#include <sched.h>
#include "cond_var.h"

void condition_variable_init(condition_variable* cv)
{
    atomic_init(&cv->ticket_head, 0);
    atomic_init(&cv->ticket_tail, 0);
    semaphore_init(&cv->sem, 0);
}

void condition_variable_wait(condition_variable* cv, ticket_lock* ext_lock)
{
    int my_ticket = atomic_fetch_add(&cv->ticket_head, 1);
    ticketlock_release(ext_lock);
    
    // Wait while our ticket is not yet admitted
    while (atomic_load(&cv->ticket_tail) <= my_ticket) {
        semaphore_wait(&cv->sem);
        
        // Case where we woke up but it's not our turn yet.
        if (atomic_load(&cv->ticket_tail) <= my_ticket) {
            semaphore_signal(&cv->sem);
            sched_yield();
        }
    }
    
    ticketlock_acquire(ext_lock);
}

void condition_variable_signal(condition_variable* cv)
{
    int tail, head;
    do {
        tail = atomic_load(&cv->ticket_tail);
        head = atomic_load(&cv->ticket_head);
        if (tail >= head) {
            return; // No waiting threads
        }
    } while (!atomic_compare_exchange_weak(&cv->ticket_tail, &tail, tail + 1));
    
    // Exactly one ticket admitted, meaning exactly one thread can pass its loop
    semaphore_signal(&cv->sem);
}

void condition_variable_broadcast(condition_variable* cv)
{
    int tail, head;
    do {
        tail = atomic_load(&cv->ticket_tail);
        head = atomic_load(&cv->ticket_head);
        if (tail >= head) {
            return; // No waiting threads
        }
    } while (!atomic_compare_exchange_weak(&cv->ticket_tail, &tail, head));
    
    // We advanced ticket_tail up to ticket_head, meaning (head - tail) threads are admitted
    int to_wake = head - tail;
    for (int i = 0; i < to_wake; i++) {
        semaphore_signal(&cv->sem);
    }
}