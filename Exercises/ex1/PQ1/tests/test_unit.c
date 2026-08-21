#include <stdio.h>

#include "../tl_semaphore.h"

int main() {
    semaphore sem;
    
    semaphore_init(&sem, 1);    
    semaphore_wait(&sem);    
    semaphore_signal(&sem);
    
    printf("Unit test passed successfully.\n");
    return 0;
}
