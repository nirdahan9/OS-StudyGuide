#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>
#include "rw_lock.h"

#define NUM_READERS 12
#define NUM_WRITERS 4

rwlock lock;
atomic_int total_writer_acquisitions = 0;
atomic_bool test_running = true;

// Track acquisitions per writer thread
int writer_counts[NUM_WRITERS] = {0};

void* writer_thread(void* arg) {
    int id = *(int*)arg;
    
    while (atomic_load(&test_running)) {
        rwlock_acquire_write(&lock);
        
        int current_total = atomic_fetch_add(&total_writer_acquisitions, 1);
        if (current_total >= 5000) {
            test_running = false;
            rwlock_release_write(&lock);
            break;
        }
        
        writer_counts[id]++;
        
        // Short work
        for (volatile int j = 0; j < 1000; j++) {}
        
        rwlock_release_write(&lock);
        
        // Yield to allow other threads to run
        sched_yield();
    }
    return NULL;
}

void* reader_thread(void* arg) {
    (void)arg;
    
    while (atomic_load(&test_running)) {
        rwlock_acquire_read(&lock);
        
        // Short work
        for (volatile int j = 0; j < 1000; j++) {}
        
        rwlock_release_read(&lock);
        
        // Tiny yield
        sched_yield();
    }
    return NULL;
}

int main() {
    rwlock_init(&lock);
    
    pthread_t readers[NUM_READERS];
    pthread_t writers[NUM_WRITERS];
    int writer_ids[NUM_WRITERS];
    
    for (int i = 0; i < NUM_WRITERS; i++) {
        writer_ids[i] = i;
        pthread_create(&writers[i], NULL, writer_thread, &writer_ids[i]);
    }
    
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader_thread, NULL);
    }
    
    // Wait for writers
    for (int i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writers[i], NULL);
    }
    
    // Force readers to stop and join them
    test_running = false;
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    
    bool all_passed = true;
    for (int i = 0; i < NUM_WRITERS; i++) {
        if (writer_counts[i] < 100) {
            all_passed = false;
            printf("FAIL: Writer %d starved. Only %d out of 5000 acquisitions.\n", i, writer_counts[i]);
        }
    }
    
    if (all_passed) {
        printf("PASS: No writer starvation. Acquisitions per writer: [%d, %d, %d, %d]\n", 
               writer_counts[0], writer_counts[1], writer_counts[2], writer_counts[3]);
        return 0;
    }
    
    return 1;
}
