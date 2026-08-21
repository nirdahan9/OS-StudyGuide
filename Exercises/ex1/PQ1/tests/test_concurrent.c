#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "../tl_semaphore.h"

semaphore my_sem;

void* thread_func(void* arg) {
    semaphore_wait(&my_sem);
    return NULL;
}

int main() {
    pthread_t threads[8];
    
    semaphore_init(&my_sem, 0);
    
    // Create 8 threads
    const int num_threads = 8;
    for(int i = 0; i < num_threads; i++) {
        if(pthread_create(&threads[i], NULL, thread_func, NULL) != 0) {
            printf("Thread creation failed\n");
            return 1;
        }
    }
    
    // To ensure threads are waiting
    sleep(1);
    
    // Signal 8 times, then wait for all threads to finish
    for(int i = 0; i < num_threads; i++) {
        semaphore_signal(&my_sem);
    }
    for(int i = 0; i < num_threads; i++) {
        if(pthread_join(threads[i], NULL) != 0) {
            printf("Thread join failed\n");
            return 1;
        }
    }
    
    printf("Concurrency test passed successfully.\n");
    return 0;
}
