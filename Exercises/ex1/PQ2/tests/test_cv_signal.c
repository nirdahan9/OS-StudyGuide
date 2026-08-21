#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include "cond_var.h"

#define NUM_THREADS 8

condition_variable cv;
ticket_lock ext_lock;
atomic_int ready_count;
atomic_int released_count;

void* thread_func(void* arg) {
    (void)arg;

    // Methods each thread will do
    ticketlock_acquire(&ext_lock);
    atomic_fetch_add(&ready_count, 1);    
    condition_variable_wait(&cv, &ext_lock);
    atomic_fetch_add(&released_count, 1);    
    ticketlock_release(&ext_lock);
    return NULL;
}

int main() {
    condition_variable_init(&cv);
    ticketlock_init(&ext_lock);
    atomic_init(&ready_count, 0);
    atomic_init(&released_count, 0);

    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, thread_func, NULL) != 0) {
            printf("Failed to create thread.\n");
            return 1;
        }
    }

    // Wait until all 8 threads have reached the ready point
    while (atomic_load(&ready_count) < NUM_THREADS) {
        sched_yield();
    }
    
    // Signal one by one and verify that exactly one thread is released each time
    ticketlock_acquire(&ext_lock);
    ticketlock_release(&ext_lock);

    for (int i = 1; i <= NUM_THREADS; i++) {
        condition_variable_signal(&cv);
        
        // Verify that after the i-th signal, exactly i threads are released
        int retries = 0;
        while (atomic_load(&released_count) != i) {
            if (retries++ > 1000000) {
                printf("Failure: expected %d threads released, but got %d\n", i, atomic_load(&released_count));
                return 1;
            }
            sched_yield();
        }
    }

    // Join all threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Signal test passed.\n");
    return 0;
}
