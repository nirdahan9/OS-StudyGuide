#include <stdio.h>
#include "cond_var.h"

int main() {
    condition_variable cv;
    condition_variable_init(&cv);
    
    condition_variable_signal(&cv);
    condition_variable_broadcast(&cv);
    
    printf("Unit test passed.\n");
    return 0;
}
