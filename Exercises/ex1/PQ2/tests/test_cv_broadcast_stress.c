#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include "cond_var.h"

#define NUM_THREADS 16
#define NUM_ITERATIONS 500

condition_variable cv;
ticket_lock ext_lock;
atomic_int ready_count;
atomic_int released_count;

void* thread_func(void* arg) {
    (void)arg;
    for (int k = 0; k < NUM_ITERATIONS; k++) {
        // Same as test_cv_signal , in loop to increase stress and catch more edge cases.
        ticketlock_acquire(&ext_lock);
        atomic_fetch_add(&ready_count, 1);
        condition_variable_wait(&cv, &ext_lock);
        atomic_fetch_add(&released_count, 1);
        ticketlock_release(&ext_lock);
    }
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

    for (int k = 1; k <= NUM_ITERATIONS; k++) {
        // Wait until all 16 threads are ready for this iteration
        int expected_ready = k * NUM_THREADS;
        while (atomic_load(&ready_count) < expected_ready) {
            sched_yield();
        }
        
        // To ensure the last thread has entered condition_variable_wait and released the lock.
        ticketlock_acquire(&ext_lock);
        ticketlock_release(&ext_lock);

        condition_variable_broadcast(&cv);

        // Verify all 16 threads were released in this iteration
        int expected_released = k * NUM_THREADS;
        int retries = 0;
        while (atomic_load(&released_count) < expected_released) {
            if (retries++ > 5000000) {
                printf("Failure: Deadlock or missed wakeup at iteration %d\n", k);
                return 1;
            }
            sched_yield();
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Broadcast stress test passed.\n");
    return 0;
}
