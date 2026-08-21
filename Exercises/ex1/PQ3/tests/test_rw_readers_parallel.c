#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include "rw_lock.h"

#define NUM_THREADS 8

rwlock lock;
atomic_int concurrent_readers = 0;
atomic_int max_concurrent_readers = 0;

void* reader_thread(void* arg) {
    (void)arg;
    
    rwlock_acquire_read(&lock);
    
    // Increment the number of readers currently in the CS and get the current count
    int current = atomic_fetch_add(&concurrent_readers, 1) + 1;
    
    // Update the maximum observed concurrent readers
    int max = atomic_load(&max_concurrent_readers);
    while (current > max && !atomic_compare_exchange_weak(&max_concurrent_readers, &max, current)) {
        // keep trying to update if max changes concurrently
    }
    
    // Wait actively to allow other threads to enter
    int spin_count = 0;
    while(atomic_load(&concurrent_readers) < 2 && spin_count++ < 100000) {
        sched_yield();
    }
    
    // Decrement as we leave
    atomic_fetch_sub(&concurrent_readers, 1);
    
    rwlock_release_read(&lock);
    return NULL;
}

int main() {
    rwlock_init(&lock);
    
    pthread_t threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_create(&threads[i], NULL, reader_thread, NULL) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }
    
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    int max_readers = atomic_load(&max_concurrent_readers);
    
    // Verify at least 2 readers inside simultaneously.
    if (max_readers >= 2) {
        printf("PASS: Readers ran together. Max concurrent readers = %d (>= 2)\n", max_readers);
        return 0;
    } else {
        printf("FAIL: Readers did not run together. Max concurrent readers = %d (< 2)\n", max_readers);
        return 1;
    }
}
