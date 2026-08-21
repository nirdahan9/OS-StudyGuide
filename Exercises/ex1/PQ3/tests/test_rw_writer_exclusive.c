#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdatomic.h>
#include "rw_lock.h"

#define NUM_READERS 8

rwlock lock;
atomic_int readers_in_cs = 0;
atomic_int writers_in_cs = 0;
atomic_bool overlap_detected = false;
atomic_bool test_running = true;

void* writer_thread(void* arg) {
    (void)arg;
    
    for (int i = 0; i < 10; i++) {
        rwlock_acquire_write(&lock);
        
        atomic_fetch_add(&writers_in_cs, 1);
        
        // CS checks: No readers inside, no other writer inside
        if (atomic_load(&readers_in_cs) > 0 || atomic_load(&writers_in_cs) > 1) {
            overlap_detected = true;
        }
        
        // Simulate work while holding the write lock
        sleep(50000);
        
        // Verify again just before releasing
        if (atomic_load(&readers_in_cs) > 0 || atomic_load(&writers_in_cs) > 1) {
            overlap_detected = true;
        }
        
        atomic_fetch_sub(&writers_in_cs, 1);
        
        rwlock_release_write(&lock);

        // Rest before trying again
        sleep(10000);
    }
    
    // Writer finished its work -> end the loop for readers
    test_running = false;
    return NULL;
}

void* reader_thread(void* arg) {
    (void)arg;
    
    while (atomic_load(&test_running)) {
        rwlock_acquire_read(&lock);
        
        atomic_fetch_add(&readers_in_cs, 1);
        
        // Ensure no writer is inside while we read
        if (atomic_load(&writers_in_cs) > 0) {
            overlap_detected = true;
        }
        
        // Short reading operation
        sleep(10000);
        
        atomic_fetch_sub(&readers_in_cs, 1);
        
        rwlock_release_read(&lock);
        
        // Pause before next read
        sleep(5000);
    }
    return NULL;
}

int main() {
    rwlock_init(&lock);
    
    pthread_t writer;
    pthread_t readers[NUM_READERS];
    
    pthread_create(&writer, NULL, writer_thread, NULL);
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_create(&readers[i], NULL, reader_thread, NULL);
    }
    
    pthread_join(writer, NULL);
    for (int i = 0; i < NUM_READERS; i++) {
        pthread_join(readers[i], NULL);
    }
    
    if (overlap_detected) {
        printf("FAIL: Exclusivity violated! Overlap detected between writer/reader or writer/writer.\n");
        return 1;
    } else {
        printf("PASS: Strict writer exclusivity maintained.\n");
        return 0;
    }
}
