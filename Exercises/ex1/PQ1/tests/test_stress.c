#include <stdio.h>
#include <pthread.h>

#include "../tl_semaphore.h"

#define NUM_THREADS 16
#define NUM_ITERATIONS 10000

semaphore count_sem;
int resource_counter = 0;

void* stress_func(void* arg) {
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        semaphore_wait(&count_sem);
        resource_counter++;
        semaphore_signal(&count_sem);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    
    semaphore_init(&count_sem, 1);
    
    // Create & join threads
    for (int i = 0; i < NUM_THREADS; i++) {
        if(pthread_create(&threads[i], NULL, stress_func, NULL) != 0) {
            printf("Thread %d creation failed\n", i);
            return 1;
        }
    }    
    for (int i = 0; i < NUM_THREADS; i++) {
        if(pthread_join(threads[i], NULL) != 0) {
            printf("Thread %d join failed\n", i);
            return 1;
        }
    }
    
    // Ensure accurate counting (no mutex bugs)
    if (resource_counter != NUM_THREADS * NUM_ITERATIONS) {
        printf("Stress test failed: Expected counter = %d, Actual = %d\n", NUM_THREADS * NUM_ITERATIONS, resource_counter);
        return 1;
    }
    
    printf("Stress test passed successfully.\n");
    return 0;
}
